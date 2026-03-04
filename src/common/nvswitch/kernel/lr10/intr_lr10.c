/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "intr_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/minion_lr10.h"
#include "regkey_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "inforom/inforom_nvswitch.h"

#include "nvswitch/lr10/dev_nvs.h"
#include "nvswitch/lr10/dev_nvs_master.h"
#include "nvswitch/lr10/dev_timer.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_pri_ringmaster.h"
#include "nvswitch/lr10/dev_pri_ringstation_sys.h"
#include "nvswitch/lr10/dev_pri_ringstation_prt.h"
#include "nvswitch/lr10/dev_nv_xve.h"
#include "nvswitch/lr10/dev_npg_ip.h"
#include "nvswitch/lr10/dev_nport_ip.h"
#include "nvswitch/lr10/dev_route_ip.h"
#include "nvswitch/lr10/dev_ingress_ip.h"
#include "nvswitch/lr10/dev_sourcetrack_ip.h"
#include "nvswitch/lr10/dev_egress_ip.h"
#include "nvswitch/lr10/dev_tstate_ip.h"
#include "nvswitch/lr10/dev_nxbar_tc_global_ip.h"
#include "nvswitch/lr10/dev_nxbar_tile_ip.h"
#include "nvswitch/lr10/dev_nvlipt_ip.h"
#include "nvswitch/lr10/dev_nvltlc_ip.h"
#include "nvswitch/lr10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/lr10/dev_minion_ip.h"
#include "nvswitch/lr10/dev_nvldl_ip.h"
#include "nvswitch/lr10/dev_nvltlc_ip.h"
#include "nvswitch/lr10/dev_nvlctrl_ip.h"

static void
_nvswitch_construct_ecc_error_event
(
    INFOROM_NVS_ECC_ERROR_EVENT *err_event,
    NvU32  sxid,
    NvU32  linkId,
    NvBool bAddressValid,
    NvU32  address,
    NvBool bUncErr,
    NvU32  errorCount
)
{
    err_event->sxid          = sxid;
    err_event->linkId        = linkId;
    err_event->bAddressValid = bAddressValid;
    err_event->address       = address;
    err_event->bUncErr       = bUncErr;
    err_event->errorCount    = errorCount;
}

/*
 * @Brief : Enable top level HW interrupts.
 *
 * @Description :
 *
 * @param[in] device        operate on this device
 */
void
nvswitch_lib_enable_interrupts_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 saw_legacy_intr_enable = 0;

    if (FLD_TEST_DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PTIMER, 1, chip_device->intr_enable_legacy))
    {
        saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PTIMER_0, 1, saw_legacy_intr_enable);
        saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PTIMER_1, 1, saw_legacy_intr_enable);
    }
    if (FLD_TEST_DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PMGR, 1, chip_device->intr_enable_legacy))
    {
        saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PMGR_0, 1, saw_legacy_intr_enable);
        saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PMGR_1, 1, saw_legacy_intr_enable);
    }

    NVSWITCH_REG_WR32(device, _PSMC, _INTR_EN_SET_LEGACY, chip_device->intr_enable_legacy);
    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, saw_legacy_intr_enable);
}

/*
 * @Brief : Disable top level HW interrupts.
 *
 * @Description :
 *
 * @param[in] device        operate on this device
 */
void
nvswitch_lib_disable_interrupts_lr10
(
    nvswitch_device *device
)
{
    if (NVSWITCH_GET_CHIP_DEVICE_LR10(device) == NULL)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Can not disable interrupts.  Chip device==NULL\n",
            __FUNCTION__);
        return;
    }

    NVSWITCH_REG_WR32(device, _PSMC, _INTR_EN_CLR_LEGACY, 0xffffffff);

    //
    // Need a bit more time to ensure interrupt de-asserts, on
    // RTL simulation. Part of BUG 1869204 and 1881361.
    //
    if (IS_RTLSIM(device))
    {
        (void)NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_EN_CLR_CORRECTABLE);
    }
}

static void
_nvswitch_build_top_interrupt_mask_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 intr_bit;
    NvU32 i;

    chip_device->intr_enable_legacy =
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PTIMER, 1) |
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PMGR, 1) |
        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _SAW, 1) |
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _DECODE_TRAP_PRIV_LEVEL_VIOLATION, 1) |
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _DECODE_TRAP_WRITE_DROPPED, 1) |
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _RING_MANAGE_SUCCESS, 1) |
        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PBUS, 1) |
//        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _XVE, 1) |
        DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PRIV_RING, 1) |
        0;

    chip_device->intr_enable_fatal = 0;
    chip_device->intr_enable_nonfatal = 0;
    chip_device->intr_enable_corr = 0;

    for (i = 0; i < NUM_NXBAR_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NXBAR, i))
        {
            intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_FATAL, _NXBAR_0, 1) << i;

            // NXBAR only has fatal interrupts
            chip_device->intr_enable_fatal |= intr_bit;
        }
    }

    for (i = 0; i < NUM_NPG_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NPG, i))
        {
            intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_FATAL, _NPG_0, 1) << i;
            chip_device->intr_enable_fatal |= intr_bit;

            intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_NONFATAL, _NPG_0, 1) << i;
            chip_device->intr_enable_nonfatal |= intr_bit;
        }
    }

    for (i = 0; i < NUM_NVLW_ENGINE_LR10; i++)
    {
        if (NVSWITCH_ENG_VALID_LR10(device, NVLW, i))
        {
            intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_FATAL, _NVLIPT_0, 1) << i;
            chip_device->intr_enable_fatal |= intr_bit;

            intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_NONFATAL, _NVLIPT_0, 1) << i;
            chip_device->intr_enable_nonfatal |= intr_bit;
        }
    }

#if defined(NV_NVLSAW_NVSPMC_INTR_EN_SET_FATAL_SOE)
    if (nvswitch_is_soe_supported(device))
    {
        intr_bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_FATAL, _SOE, 1);
        chip_device->intr_enable_fatal |= intr_bit;
    }
#endif
}

static void
_nvswitch_initialize_minion_interrupts
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvU32 intrEn, localDiscoveredLinks, globalLink, i;
    localDiscoveredLinks = 0;

    // Tree 1 (non-stall) is disabled until there is a need
    NVSWITCH_MINION_WR32_LR10(device, instance, _MINION, _MINION_INTR_NONSTALL_EN, 0);

     // Tree 0 (stall) is where we route _all_ MINION interrupts for now
    intrEn = DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FATAL,          _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _NONFATAL,       _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FALCON_STALL,   _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FALCON_NOSTALL, _DISABLE);

    for (i = 0; i < NVSWITCH_LINKS_PER_MINION; ++i)
    {
        // get the global link number of the link we are iterating over
        globalLink = (instance * NVSWITCH_LINKS_PER_MINION) + i;

        // the link is valid place bit in link mask
        if (device->link[globalLink].valid)
        {
            localDiscoveredLinks |= NVBIT(i);
        }
    }

    intrEn = FLD_SET_DRF_NUM(_MINION, _MINION_INTR_STALL_EN, _LINK,
                            localDiscoveredLinks, intrEn);

    NVSWITCH_MINION_WR32_LR10(device, instance, _MINION, _MINION_INTR_STALL_EN, intrEn);
}

static void
_nvswitch_initialize_nvlipt_interrupts_lr10
(
    nvswitch_device *device
)
{
    NvU32 i;
    NvU32 regval = 0;

    //
    // NVLipt interrupt routing (NVLIPT_COMMON, NVLIPT_LNK, NVLDL, NVLTLC)
    // will be initialized by MINION NVLPROD flow
    //
    // We must enable interrupts at the top levels in NVLW, NVLIPT_COMMON,
    // NVLIPT_LNK and MINION
    //

    // NVLW
    regval = DRF_NUM(_NVLCTRL_COMMON, _INTR_0_MASK, _FATAL,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_0_MASK, _NONFATAL,    0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_0_MASK, _CORRECTABLE, 0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_0_MASK, _INTR0,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_0_MASK, _INTR1,       0x1);
    NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_COMMON, _INTR_0_MASK, regval);

    regval = DRF_NUM(_NVLCTRL_COMMON, _INTR_1_MASK, _FATAL,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_1_MASK, _NONFATAL,    0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_1_MASK, _CORRECTABLE, 0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_1_MASK, _INTR0,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_1_MASK, _INTR1,       0x1);
    NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_COMMON, _INTR_1_MASK, regval);

    regval = DRF_NUM(_NVLCTRL_COMMON, _INTR_2_MASK, _FATAL,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_2_MASK, _NONFATAL,    0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_2_MASK, _CORRECTABLE, 0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_2_MASK, _INTR0,       0x1) |
             DRF_NUM(_NVLCTRL_COMMON, _INTR_2_MASK, _INTR1,       0x1);
    NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_COMMON, _INTR_2_MASK, regval);

    // NVLW link
    for (i = 0; i < NV_NVLCTRL_LINK_INTR_0_MASK__SIZE_1; i++)
    {
        regval = DRF_NUM(_NVLCTRL_LINK, _INTR_0_MASK, _FATAL,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_0_MASK, _NONFATAL,    0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_0_MASK, _CORRECTABLE, 0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_0_MASK, _INTR0,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_0_MASK, _INTR1,       0x1);
        NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_LINK, _INTR_0_MASK(i), regval);

        regval = DRF_NUM(_NVLCTRL_LINK, _INTR_1_MASK, _FATAL,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_1_MASK, _NONFATAL,    0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_1_MASK, _CORRECTABLE, 0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_1_MASK, _INTR0,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_1_MASK, _INTR1,       0x1);
        NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_LINK, _INTR_1_MASK(i), regval);

        regval = DRF_NUM(_NVLCTRL_LINK, _INTR_2_MASK, _FATAL,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_2_MASK, _NONFATAL,    0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_2_MASK, _CORRECTABLE, 0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_2_MASK, _INTR0,       0x1) |
                 DRF_NUM(_NVLCTRL_LINK, _INTR_2_MASK, _INTR1,       0x1);
        NVSWITCH_BCAST_WR32_LR10(device, NVLW, _NVLCTRL_LINK, _INTR_2_MASK(i), regval);
    }

    // NVLIPT_COMMON
    regval = DRF_NUM(_NVLIPT_COMMON, _INTR_CONTROL_COMMON, _INT0_EN, 0x1) |
             DRF_NUM(_NVLIPT_COMMON, _INTR_CONTROL_COMMON, _INT1_EN, 0x1);

    NVSWITCH_BCAST_WR32_LR10(device, NVLIPT, _NVLIPT_COMMON, _INTR_CONTROL_COMMON, regval);

    // NVLIPT_LNK
    regval = DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT0_EN, 0x1) |
             DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT1_EN, 0x1);
    NVSWITCH_BCAST_WR32_LR10(device, NVLIPT_LNK, _NVLIPT_LNK, _INTR_CONTROL_LINK, regval);

    // MINION
    for (i = 0; i < NUM_MINION_ENGINE_LR10; ++i)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, MINION, i))
        {
            continue;
        }

        _nvswitch_initialize_minion_interrupts(device,i);
    }
}

static void
_nvswitch_initialize_nxbar_tileout_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 report_fatal;
    NvU32 tileout;

    report_fatal =
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _INGRESS_BUFFER_OVERFLOW, 1)     |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _INGRESS_BUFFER_UNDERFLOW, 1)    |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _EGRESS_CREDIT_OVERFLOW, 1)      |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _EGRESS_CREDIT_UNDERFLOW, 1)     |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _INGRESS_NON_BURSTY_PKT, 1)      |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _INGRESS_NON_STICKY_PKT, 1)      |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _INGRESS_BURST_GT_9_DATA_VC, 1)  |
        DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_FATAL_INTR_EN, _EGRESS_CDT_PARITY_ERROR, 1);

    for (tileout = 0; tileout < NUM_NXBAR_TILEOUTS_PER_TC_LR10; tileout++)
    {
        NVSWITCH_BCAST_WR32_LR10(device, NXBAR, _NXBAR, _TC_TILEOUT_ERR_FATAL_INTR_EN(tileout), report_fatal);
    }

    chip_device->intr_mask.tileout.fatal = report_fatal;
}

static void
_nvswitch_initialize_nxbar_tile_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 report_fatal;

    report_fatal =
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_BUFFER_OVERFLOW, 1)     |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_BUFFER_UNDERFLOW, 1)    |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _EGRESS_CREDIT_OVERFLOW, 1)      |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _EGRESS_CREDIT_UNDERFLOW, 1)     |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_NON_BURSTY_PKT, 1)      |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_NON_STICKY_PKT, 1)      |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_BURST_GT_9_DATA_VC, 1)  |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_PKT_INVALID_DST, 1)     |
        DRF_NUM(_NXBAR, _TILE_ERR_FATAL_INTR_EN, _INGRESS_PKT_PARITY_ERROR, 1);

    NVSWITCH_BCAST_WR32_LR10(device, TILE, _NXBAR, _TILE_ERR_FATAL_INTR_EN, report_fatal);

    chip_device->intr_mask.tile.fatal = report_fatal;
}

static void
_nvswitch_initialize_nxbar_interrupts
(
    nvswitch_device *device
)
{
    _nvswitch_initialize_nxbar_tile_interrupts(device);
    _nvswitch_initialize_nxbar_tileout_interrupts(device);
}

static void
_nvswitch_initialize_route_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 enable;
    NvU32 report_fatal;
    NvU32 report_nonfatal;
    NvU32 contain;

    report_fatal =
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _ROUTEBUFERR, _ENABLE)            |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _NOPORTDEFINEDERR, _DISABLE)      |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _INVALIDROUTEPOLICYERR, _DISABLE) |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _GLT_ECC_LIMIT_ERR, _DISABLE)     |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _GLT_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _TRANSDONERESVERR, _DISABLE)      |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _PDCTRLPARERR, _ENABLE)           |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _NVS_ECC_LIMIT_ERR, _DISABLE)     |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _NVS_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _CDTPARERR, _ENABLE);

    report_nonfatal =
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _ROUTEBUFERR, _DISABLE)          |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _NOPORTDEFINEDERR, _ENABLE)      |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _INVALIDROUTEPOLICYERR, _ENABLE) |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _GLT_ECC_LIMIT_ERR, _DISABLE)    |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _GLT_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _TRANSDONERESVERR, _DISABLE)     |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _PDCTRLPARERR, _DISABLE)         |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _NVS_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _NVS_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _CDTPARERR, _DISABLE);

    contain =
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _ROUTEBUFERR, __PROD)           |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _NOPORTDEFINEDERR, __PROD)      |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _INVALIDROUTEPOLICYERR, __PROD) |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _GLT_ECC_LIMIT_ERR, __PROD)     |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _GLT_ECC_DBE_ERR, __PROD)       |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _TRANSDONERESVERR, __PROD)      |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _PDCTRLPARERR, __PROD)          |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _NVS_ECC_LIMIT_ERR, __PROD)     |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _NVS_ECC_DBE_ERR, __PROD)       |
        DRF_DEF(_ROUTE, _ERR_CONTAIN_EN_0, _CDTPARERR, __PROD);

    enable = report_fatal | report_nonfatal;
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ERR_LOG_EN_0, enable);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ERR_FATAL_REPORT_EN_0, report_fatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ERR_NON_FATAL_REPORT_EN_0, report_nonfatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ERR_CORRECTABLE_REPORT_EN_0, 0);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ERR_CONTAIN_EN_0, contain);

    chip_device->intr_mask.route.fatal = report_fatal;
    chip_device->intr_mask.route.nonfatal = report_nonfatal;
}

static void
_nvswitch_initialize_ingress_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 enable;
    NvU32 report_fatal;
    NvU32 report_nonfatal;
    NvU32 contain;

    report_fatal =
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _CMDDECODEERR, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_DBE_ERR, _ENABLE)    |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _INVALIDVCSET, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _REMAPTAB_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RIDTAB_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RLANTAB_ECC_DBE_ERR, _ENABLE)       |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_PARITY_ERR, _ENABLE)         |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _REQCONTEXTMISMATCHERR, _DISABLE)    |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _ACLFAIL, _DISABLE)                  |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_LIMIT_ERR, _DISABLE) |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _ADDRBOUNDSERR, _DISABLE)            |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RIDTABCFGERR, _DISABLE)             |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RLANTABCFGERR, _DISABLE)            |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _REMAPTAB_ECC_LIMIT_ERR, _DISABLE)   |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RIDTAB_ECC_LIMIT_ERR, _DISABLE)     |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RLANTAB_ECC_LIMIT_ERR, _DISABLE)    |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _ADDRTYPEERR, _DISABLE);

    report_nonfatal =
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REQCONTEXTMISMATCHERR, _ENABLE)    |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ACLFAIL, _ENABLE)                  |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_LIMIT_ERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRBOUNDSERR, _ENABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RIDTABCFGERR, _ENABLE)             |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RLANTABCFGERR, _ENABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REMAPTAB_ECC_LIMIT_ERR, _DISABLE)  |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RIDTAB_ECC_LIMIT_ERR, _DISABLE)    |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RLANTAB_ECC_LIMIT_ERR, _DISABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRTYPEERR, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _CMDDECODEERR, _DISABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_DBE_ERR, _DISABLE)  |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _INVALIDVCSET, _DISABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REMAPTAB_ECC_DBE_ERR, _DISABLE)    |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RIDTAB_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RLANTAB_ECC_DBE_ERR, _DISABLE)     |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOC_PARITY_ERR, _DISABLE);

    contain =
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _REQCONTEXTMISMATCHERR, __PROD)    |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _ACLFAIL, __PROD)                  |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _NCISOC_HDR_ECC_LIMIT_ERR, __PROD) |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _ADDRBOUNDSERR, __PROD)            |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RIDTABCFGERR, __PROD)             |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RLANTABCFGERR, __PROD)            |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _REMAPTAB_ECC_LIMIT_ERR, __PROD)   |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RIDTAB_ECC_LIMIT_ERR, __PROD)     |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RLANTAB_ECC_LIMIT_ERR, __PROD)    |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _ADDRTYPEERR, __PROD)              |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _CMDDECODEERR, __PROD)             |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _NCISOC_HDR_ECC_DBE_ERR, __PROD)   |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _INVALIDVCSET, __PROD)             |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _REMAPTAB_ECC_DBE_ERR, __PROD)     |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RIDTAB_ECC_DBE_ERR, __PROD)       |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _RLANTAB_ECC_DBE_ERR, __PROD)      |
        DRF_DEF(_INGRESS, _ERR_CONTAIN_EN_0, _NCISOC_PARITY_ERR, __PROD);

    enable = report_fatal | report_nonfatal;
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _INGRESS, _ERR_LOG_EN_0, enable);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _INGRESS, _ERR_FATAL_REPORT_EN_0, report_fatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0, report_nonfatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _INGRESS, _ERR_CORRECTABLE_REPORT_EN_0, 0);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _INGRESS, _ERR_CONTAIN_EN_0, contain);

    chip_device->intr_mask.ingress.fatal = report_fatal;
    chip_device->intr_mask.ingress.nonfatal = report_nonfatal;
}

static void
_nvswitch_initialize_egress_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 enable;
    NvU32 report_fatal;
    NvU32 report_nonfatal;
    NvU32 contain;

    report_fatal =
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _EGRESSBUFERR, _ENABLE)                 |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _PKTROUTEERR, _ENABLE)                  |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _SEQIDERR, _ENABLE)                     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_LIMIT_ERR, _DISABLE)     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, _DISABLE)   |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOCCREDITOVFL, _ENABLE)             |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _REQTGTIDMISMATCHERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RSPREQIDMISMATCHERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _URRSPERR, _DISABLE)                    |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _PRIVRSPERR, _DISABLE)                  |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _HWRSPERR, _DISABLE)                    |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_HDR_PARITY_ERR, _ENABLE)         |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_CREDIT_PARITY_ERR, _ENABLE)     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_FLITTYPE_MISMATCH_ERR, _ENABLE)  |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _CREDIT_TIME_OUT_ERR, _ENABLE);

    report_nonfatal =
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EGRESSBUFERR, _DISABLE)                 |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _PKTROUTEERR, _DISABLE)                  |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _SEQIDERR, _DISABLE)                     |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_LIMIT_ERR, _ENABLE)       |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_DBE_ERR, _DISABLE)        |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOCCREDITOVFL, _DISABLE)             |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REQTGTIDMISMATCHERR, _DISABLE)          |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RSPREQIDMISMATCHERR, _DISABLE)          |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _URRSPERR, _ENABLE)                      |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _PRIVRSPERR, _ENABLE)                    |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _HWRSPERR, _ENABLE)                      |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NXBAR_HDR_PARITY_ERR, _DISABLE)         |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOC_CREDIT_PARITY_ERR, _DISABLE)     |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NXBAR_FLITTYPE_MISMATCH_ERR, _DISABLE)  |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _CREDIT_TIME_OUT_ERR, _DISABLE);

    contain =
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _EGRESSBUFERR, __PROD)                 |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _PKTROUTEERR, __PROD)                  |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _SEQIDERR, __PROD)                     |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NXBAR_HDR_ECC_LIMIT_ERR, __PROD)      |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NXBAR_HDR_ECC_DBE_ERR, __PROD)        |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, __PROD)    |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _RAM_OUT_HDR_ECC_DBE_ERR, __PROD)      |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NCISOCCREDITOVFL, __PROD)             |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _REQTGTIDMISMATCHERR, __PROD)          |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _RSPREQIDMISMATCHERR, __PROD)          |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _URRSPERR, __PROD)                     |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _PRIVRSPERR, __PROD)                   |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _HWRSPERR, __PROD)                     |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NXBAR_HDR_PARITY_ERR, __PROD)         |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NCISOC_CREDIT_PARITY_ERR, __PROD)     |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _NXBAR_FLITTYPE_MISMATCH_ERR, __PROD)  |
        DRF_DEF(_EGRESS, _ERR_CONTAIN_EN_0, _CREDIT_TIME_OUT_ERR, __PROD);

    enable = report_fatal | report_nonfatal;

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _ERR_LOG_EN_0, enable);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _ERR_FATAL_REPORT_EN_0, report_fatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0, report_nonfatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _ERR_CORRECTABLE_REPORT_EN_0, 0);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _ERR_CONTAIN_EN_0, contain);

    chip_device->intr_mask.egress.fatal = report_fatal;
    chip_device->intr_mask.egress.nonfatal = report_nonfatal;
}

static void
_nvswitch_initialize_tstate_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 enable;
    NvU32 report_fatal;
    NvU32 report_nonfatal;
    NvU32 contain;

    // TD_TID errors are disbaled on both fatal & non-fatal trees since TD_TID RAM is no longer used.

    report_fatal =
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOLBUFERR, _ENABLE)              |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOL_ECC_LIMIT_ERR, _DISABLE)     |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOL_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTOREBUFERR, _ENABLE)           |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, _DISABLE)  |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_DBE_ERR, _ENABLE)     |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TD_TID_RAMBUFERR, _DISABLE)          |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TD_TID_RAM_ECC_LIMIT_ERR, _DISABLE)  |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TD_TID_RAM_ECC_DBE_ERR, _DISABLE)    |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _ATO_ERR, _ENABLE)                    |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CAMRSP_ERR, _ENABLE);

    report_nonfatal =
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOLBUFERR, _DISABLE)             |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOL_ECC_LIMIT_ERR, _ENABLE)      |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOL_ECC_DBE_ERR, _DISABLE)       |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTOREBUFERR, _DISABLE)          |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_DBE_ERR, _DISABLE)    |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TD_TID_RAMBUFERR, _DISABLE)          |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TD_TID_RAM_ECC_LIMIT_ERR, _DISABLE)  |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TD_TID_RAM_ECC_DBE_ERR, _DISABLE)    |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _ATO_ERR, _DISABLE)                   |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CAMRSP_ERR, _DISABLE);

    contain =
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TAGPOOLBUFERR, __PROD)             |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TAGPOOL_ECC_LIMIT_ERR, __PROD)     |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TAGPOOL_ECC_DBE_ERR, __PROD)       |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _CRUMBSTOREBUFERR, __PROD)          |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, __PROD)  |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _CRUMBSTORE_ECC_DBE_ERR, __PROD)    |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TD_TID_RAMBUFERR, __PROD)          |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TD_TID_RAM_ECC_LIMIT_ERR, __PROD)  |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _TD_TID_RAM_ECC_DBE_ERR, __PROD)    |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _ATO_ERR, __PROD)                   |
        DRF_DEF(_TSTATE, _ERR_CONTAIN_EN_0, _CAMRSP_ERR, __PROD);

    enable = report_fatal | report_nonfatal;
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ERR_LOG_EN_0, enable);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ERR_FATAL_REPORT_EN_0, report_fatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ERR_NON_FATAL_REPORT_EN_0, report_nonfatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ERR_CORRECTABLE_REPORT_EN_0, 0);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ERR_CONTAIN_EN_0, contain);

    chip_device->intr_mask.tstate.fatal = report_fatal;
    chip_device->intr_mask.tstate.nonfatal = report_nonfatal;
}

static void
_nvswitch_initialize_sourcetrack_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 enable;
    NvU32 report_fatal;
    NvU32 report_nonfatal;
    NvU32 contain;

    report_fatal =
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, _DISABLE)    |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_LIMIT_ERR, _DISABLE) |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, _DISABLE)    |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, _ENABLE)       |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_DBE_ERR, _DISABLE)   |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR, _ENABLE)       |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _SOURCETRACK_TIME_OUT_ERR, _ENABLE);

    report_nonfatal =
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_LIMIT_ERR, _DISABLE) |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_DBE_ERR, _DISABLE)   |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR, _DISABLE)      |
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _SOURCETRACK_TIME_OUT_ERR, _DISABLE);

    contain =
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, __PROD)    |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_LIMIT_ERR, __PROD) |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, __PROD)    |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, __PROD)      |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_DBE_ERR, __PROD)   |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR, __PROD)      |
        DRF_DEF(_SOURCETRACK, _ERR_CONTAIN_EN_0, _SOURCETRACK_TIME_OUT_ERR, __PROD);

    enable = report_fatal | report_nonfatal;
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _ERR_LOG_EN_0, enable);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0, report_fatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, report_nonfatal);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _ERR_CORRECTABLE_REPORT_EN_0, 0);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _ERR_CONTAIN_EN_0, contain);

    chip_device->intr_mask.sourcetrack.fatal = report_fatal;
    chip_device->intr_mask.sourcetrack.nonfatal = report_nonfatal;

}

void
_nvswitch_initialize_nport_interrupts
(
    nvswitch_device *device
)
{
    NvU32 val;

    val =
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE, 1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE, 1);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _ERR_CONTROL_COMMON_NPORT, val);

    _nvswitch_initialize_route_interrupts(device);
    _nvswitch_initialize_ingress_interrupts(device);
    _nvswitch_initialize_egress_interrupts(device);
    _nvswitch_initialize_tstate_interrupts(device);
    _nvswitch_initialize_sourcetrack_interrupts(device);
}

static void
_nvswitch_initialize_saw_interrupts
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _INTR_EN_SET_CORRECTABLE, chip_device->intr_enable_corr);
    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _INTR_EN_SET_FATAL,       chip_device->intr_enable_fatal);
    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _INTR_EN_SET_NONFATAL,    chip_device->intr_enable_nonfatal);
}

/*
 * Initialize interrupt tree HW for all units.
 *
 * Init and servicing both depend on bits matching across STATUS/MASK
 * and IErr STATUS/LOG/REPORT/CONTAIN registers.
 */
void
nvswitch_initialize_interrupt_tree_lr10
(
    nvswitch_device *device
)
{
    _nvswitch_build_top_interrupt_mask_lr10(device);

    // Initialize legacy interrupt tree - depends on reset to disable
    // unused interrupts
    NVSWITCH_REG_WR32(device, _PBUS, _INTR_0, 0xffffffff);

    // Clear prior saved PRI error data
    NVSWITCH_REG_WR32(device, _PBUS, _PRI_TIMEOUT_SAVE_0,
        DRF_DEF(_PBUS, _PRI_TIMEOUT_SAVE_0, _TO, _CLEAR));

    NVSWITCH_REG_WR32(device, _PBUS, _INTR_EN_0,
            DRF_DEF(_PBUS, _INTR_EN_0, _PRI_SQUASH, _ENABLED) |
            DRF_DEF(_PBUS, _INTR_EN_0, _PRI_FECSERR, _ENABLED) |
            DRF_DEF(_PBUS, _INTR_EN_0, _PRI_TIMEOUT, _ENABLED) |
            DRF_DEF(_PBUS, _INTR_EN_0, _SW, _ENABLED));

    // SAW block
    _nvswitch_initialize_saw_interrupts(device);

    // NPG/NPORT
    _nvswitch_initialize_nport_interrupts(device);

    // NVLIPT interrupts
    _nvswitch_initialize_nvlipt_interrupts_lr10(device);

    // NXBAR interrupts
    _nvswitch_initialize_nxbar_interrupts(device);
}

/*
 * @brief Service MINION Falcon interrupts on the requested interrupt tree
 *        Falcon Interrupts are a little in unqiue in how they are handled:#include <assert.h>
 *        IRQSTAT is used to read in interrupt status from FALCON
 *        IRQMASK is used to read in mask of interrupts
 *        IRQDEST is used to read in enabled interrupts that are routed to the HOST
 *
 *        IRQSTAT & IRQMASK gives the pending interrupting on this minion 
 *
 * @param[in] device   MINION on this device
 * @param[in] instance MINION instance
 *
 */
NvlStatus
nvswitch_minion_service_falcon_interrupts_lr10
(
    nvswitch_device *device,
    NvU32           instance
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled, intr, link;

    link = instance * NVSWITCH_LINKS_PER_MINION;
    report.raw_pending = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_IRQSTAT);
    report.raw_enable = chip_device->intr_minion_dest;
    report.mask = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_IRQMASK);

    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending; 

    bit = DRF_NUM(_CMINION_FALCON, _IRQSTAT, _WDTMR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_MINION_WATCHDOG, "MINION Watchdog timer ran out", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_CMINION_FALCON, _IRQSTAT, _HALT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_MINION_HALT, "MINION HALT", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_CMINION_FALCON, _IRQSTAT, _EXTERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_MINION_EXTERR, "MINION EXTERR", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_CMINION_FALCON, _IRQSTAT, _SWGEN0, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_PRINT(device, INFO,
                      "%s: Received MINION Falcon SWGEN0 interrupt on MINION %d.\n",
                      __FUNCTION__, instance);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_CMINION_FALCON, _IRQSTAT, _SWGEN1, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_PRINT(device, INFO,
                       "%s: Received MINION Falcon SWGEN1 interrupt on MINION %d.\n",
                      __FUNCTION__, instance);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (device->link[link].fatal_error_occurred)
    {
        intr = NVSWITCH_MINION_RD32_LR10(device, instance, _MINION, _MINION_INTR_STALL_EN);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _FATAL, _DISABLE, intr);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _FALCON_STALL, _DISABLE, intr);
        NVSWITCH_MINION_WR32_LR10(device, instance, _MINION, _MINION_INTR_STALL_EN, intr);
    }

    // Write to IRQSCLR to clear status of interrupt
    NVSWITCH_MINION_WR32_LR10(device, instance, _CMINION, _FALCON_IRQSCLR, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

//
// Check if there are interrupts pending.
//
// On silicon/emulation we only use MSIs which are not shared, so this
// function does not need to be called.
//
// FSF/RTMsim does not model interrupts correctly.  The interrupt is shared
// with USB so we must check the HW status.  In addition we must disable
// interrupts to run the interrupt thread. On silicon this is done
// automatically in XVE.
//
// This is called in the ISR context by the Linux driver.  The WAR does
// access more of device outside the Linux mutex than it should. Sim only
// supports 1 device currently so these fields are safe while interrupts
// are enabled.
//
// TODO: Bug 1881361 to remove the FSF WAR
//
NvlStatus
nvswitch_lib_check_interrupts_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 saw_legacy_intr_enable = 0;
    NvU32 pending;

    if (IS_RTLSIM(device) || IS_FMODEL(device))
    {
        pending = NVSWITCH_REG_RD32(device, _PSMC, _INTR_LEGACY);
        pending &= chip_device->intr_enable_legacy;
        if (pending)
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: _PSMC, _INTR_LEGACY pending (0x%0x)\n",
                __FUNCTION__, pending);
            return -NVL_MORE_PROCESSING_REQUIRED;
        }

        if (FLD_TEST_DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PTIMER, 1, chip_device->intr_enable_legacy))
        {
            saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PTIMER_0, 1, saw_legacy_intr_enable);
            saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PTIMER_1, 1, saw_legacy_intr_enable);
        }
        if (FLD_TEST_DRF_NUM(_PSMC, _INTR_EN_SET_LEGACY, _PMGR, 1, chip_device->intr_enable_legacy))
        {
            saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PMGR_0, 1, saw_legacy_intr_enable);
            saw_legacy_intr_enable = FLD_SET_DRF_NUM(_NVLSAW_NVSPMC, _INTR_EN_SET_LEGACY, _PMGR_1, 1, saw_legacy_intr_enable);
        }

        pending = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_LEGACY);
        pending &= saw_legacy_intr_enable;
        if (pending)
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: _NVLSAW_NVSPMC, _INTR_LEGACY pending (0x%0x)\n",
                __FUNCTION__, pending);
            return -NVL_MORE_PROCESSING_REQUIRED;
        }

        // Fatal Interrupts
        pending = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_FATAL);
        pending &= chip_device->intr_enable_fatal;
        if (pending)
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: _NVLSAW_NVSPMC, _INTR_FATAL pending (0x%0x)\n",
                __FUNCTION__, pending);
            return -NVL_MORE_PROCESSING_REQUIRED;
        }

        // Non-Fatal interrupts
        pending = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_NONFATAL);
        pending &= chip_device->intr_enable_nonfatal;
        if (pending)
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: _NVLSAW_NVSPMC, _INTR_NONFATAL pending (0x%0x)\n",
                __FUNCTION__, pending);
            return -NVL_MORE_PROCESSING_REQUIRED;
        }

        return NVL_SUCCESS;
    }
    else
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }
}

/*
 * The MSI interrupt block must be re-armed after servicing interrupts. This
 * write generates an EOI, which allows further MSIs to be triggered.
 */
static void
_nvswitch_rearm_msi_lr10
(
    nvswitch_device *device
)
{
    NVSWITCH_ENG_WR32_LR10(device, XVE, , 0, _XVE_CYA, _2, 0xff);
}

static NvlStatus
_nvswitch_service_pbus_lr10
(
    nvswitch_device *device
)
{
    NvU32 pending, mask, bit, unhandled;
    NvU32 save0, save1, save3, errCode;
    NVSWITCH_PRI_TIMEOUT_ERROR_LOG_TYPE pri_timeout = { 0 };

    pending = NVSWITCH_REG_RD32(device, _PBUS, _INTR_0);
    mask = NVSWITCH_REG_RD32(device, _PBUS, _INTR_EN_0);
    pending &= mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    bit = DRF_DEF(_PBUS, _INTR_0, _PRI_SQUASH, _PENDING) |
          DRF_DEF(_PBUS, _INTR_0, _PRI_FECSERR, _PENDING) |
          DRF_DEF(_PBUS, _INTR_0, _PRI_TIMEOUT, _PENDING);

    if (nvswitch_test_flags(pending, bit))
    {
        // PRI timeout is likely not recoverable
        NVSWITCH_REG_WR32(device, _PBUS, _INTR_0,
            DRF_DEF(_PBUS, _INTR_0, _PRI_TIMEOUT, _RESET));

        save0 = NVSWITCH_REG_RD32(device, _PBUS, _PRI_TIMEOUT_SAVE_0);
        save1 = NVSWITCH_REG_RD32(device, _PBUS, _PRI_TIMEOUT_SAVE_1);
        save3 = NVSWITCH_REG_RD32(device, _PBUS, _PRI_TIMEOUT_SAVE_3);
        errCode = NVSWITCH_REG_RD32(device, _PBUS, _PRI_TIMEOUT_FECS_ERRCODE);

        pri_timeout.addr    = DRF_VAL(_PBUS, _PRI_TIMEOUT_SAVE_0, _ADDR, save0) * 4;
        pri_timeout.data    = DRF_VAL(_PBUS, _PRI_TIMEOUT_SAVE_1, _DATA, save1);
        pri_timeout.write   = DRF_VAL(_PBUS, _PRI_TIMEOUT_SAVE_0, _WRITE, save0);
        pri_timeout.dest    = DRF_VAL(_PBUS, _PRI_TIMEOUT_SAVE_0, _TO, save0);
        pri_timeout.subId   = DRF_VAL(_PBUS, _PRI_TIMEOUT_SAVE_3, _SUBID, save3);
        pri_timeout.errCode = DRF_VAL(_PBUS, _PRI_TIMEOUT_FECS_ERRCODE, _DATA, errCode);

        // Dump register values as well
        pri_timeout.raw_data[0] = save0;
        pri_timeout.raw_data[1] = save1;
        pri_timeout.raw_data[2] = save3;
        pri_timeout.raw_data[3] = errCode;

        NVSWITCH_PRINT(device, ERROR,
                    "PBUS PRI error: %s offset: 0x%x data: 0x%x to: %d, "
                    "subId: 0x%x, FECS errCode: 0x%x\n",
                    pri_timeout.write ? "write" : "read",
                    pri_timeout.addr,
                    pri_timeout.data,
                    pri_timeout.dest,
                    pri_timeout.subId,
                    pri_timeout.errCode);

        if (FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_SQUASH, _PENDING, bit))
        {
            NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_TIMEOUT, "PBUS PRI SQUASH error", NVSWITCH_PBUS_PRI_SQUASH, 0, pri_timeout);
            NVSWITCH_PRINT(device, ERROR, "PRI_SQUASH: "
                "PBUS PRI error due to pri access while target block is in reset\n");
        }

        if (FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_FECSERR, _PENDING, bit))
        {
            NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_TIMEOUT, "PBUS PRI FECSERR error", NVSWITCH_PBUS_PRI_FECSERR, 0, pri_timeout);
            NVSWITCH_PRINT(device, ERROR, "PRI_FECSERR: "
                "FECS detected the error while processing a PRI request\n");
        }

        if (FLD_TEST_DRF(_PBUS, _INTR_0, _PRI_TIMEOUT, _PENDING, bit))
        {
            NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_TIMEOUT, "PBUS PRI TIMEOUT error", NVSWITCH_PBUS_PRI_TIMEOUT, 0, pri_timeout);
            NVSWITCH_PRINT(device, ERROR, "PRI_TIMEOUT: "
                "PBUS PRI error due non-existent host register or timeout waiting for FECS\n");
        }

        // allow next error to latch
        NVSWITCH_REG_WR32(device, _PBUS, _PRI_TIMEOUT_SAVE_0,
            FLD_SET_DRF(_PBUS, _PRI_TIMEOUT_SAVE_0, _TO, _CLEAR, save0));

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_DEF(_PBUS, _INTR_0, _SW, _PENDING);
    if (nvswitch_test_flags(pending, bit))
    {
        // Useful for debugging SW interrupts
        NVSWITCH_PRINT(device, INFO, "SW intr\n");
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_REG_WR32(device, _PBUS, _INTR_0, pending);  // W1C with _RESET

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_priv_ring_lr10
(
    nvswitch_device *device
)
{
    NvU32 pending, i;
    NVSWITCH_PRI_ERROR_LOG_TYPE pri_error;

    pending = NVSWITCH_REG_RD32(device, _PPRIV_MASTER, _RING_INTERRUPT_STATUS0);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    if (FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYS, 1, pending))
    {
        pri_error.addr = NVSWITCH_REG_RD32(device, _PPRIV_SYS, _PRIV_ERROR_ADR);
        pri_error.data = NVSWITCH_REG_RD32(device, _PPRIV_SYS, _PRIV_ERROR_WRDAT);
        pri_error.info = NVSWITCH_REG_RD32(device, _PPRIV_SYS, _PRIV_ERROR_INFO);
        pri_error.code = NVSWITCH_REG_RD32(device, _PPRIV_SYS, _PRIV_ERROR_CODE);

        NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_ERROR, "PRI WRITE SYS error", NVSWITCH_PPRIV_WRITE_SYS, 0, pri_error);

        NVSWITCH_PRINT(device, ERROR,
            "SYS PRI write error addr: 0x%08x data: 0x%08x info: 0x%08x code: 0x%08x\n",
            pri_error.addr, pri_error.data,
            pri_error.info, pri_error.code);

        pending = FLD_SET_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYS, 0, pending);
    }

    for (i = 0; i < NVSWITCH_NUM_PRIV_PRT_LR10; i++)
    {
        if (DRF_VAL(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_FBP, pending) & NVBIT(i))
        {
            pri_error.addr = NVSWITCH_REG_RD32(device, _PPRIV_PRT_PRT, _PRIV_ERROR_ADR(i));
            pri_error.data = NVSWITCH_REG_RD32(device, _PPRIV_PRT_PRT, _PRIV_ERROR_WRDAT(i));
            pri_error.info = NVSWITCH_REG_RD32(device, _PPRIV_PRT_PRT, _PRIV_ERROR_INFO(i));
            pri_error.code = NVSWITCH_REG_RD32(device, _PPRIV_PRT_PRT, _PRIV_ERROR_CODE(i));

            NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_ERROR, "PRI WRITE PRT error", NVSWITCH_PPRIV_WRITE_PRT, i, pri_error);

            NVSWITCH_PRINT(device, ERROR,
                "PRT%d PRI write error addr: 0x%08x data: 0x%08x info: 0x%08x code: 0x%08x\n",
                i, pri_error.addr, pri_error.data, pri_error.info, pri_error.code);

            pending &= ~DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
                _GBL_WRITE_ERROR_FBP, NVBIT(i));
        }
    }

    if (pending != 0)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_PRIV_ERROR, 
            "Fatal, Unexpected PRI error\n");
        NVSWITCH_LOG_FATAL_DATA(device, _HW, _HW_HOST_PRIV_ERROR, 2, 0, NV_FALSE, &pending);

        NVSWITCH_PRINT(device, ERROR,
            "Unexpected PRI error 0x%08x\n", pending);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    // TODO reset the priv ring like GPU driver?

    // acknowledge the interrupt to the ringmaster
    nvswitch_ring_master_cmd_lr10(device,
        DRF_DEF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _ACK_INTERRUPT));

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_collect_nport_error_info_lr10
(
    nvswitch_device    *device,
    NvU32               link,
    NVSWITCH_RAW_ERROR_LOG_TYPE *data,
    NvU32               *idx,
    NvU32               register_start,
    NvU32               register_end
)
{
    NvU32 register_block_size;
    NvU32 i = *idx;

    if ((register_start > register_end) ||
        (register_start % sizeof(NvU32) != 0) ||
        (register_end % sizeof(NvU32) != 0))
    {
        return -NVL_BAD_ARGS;
    }

    register_block_size = (register_end - register_start)/sizeof(NvU32) + 1;
    if ((i + register_block_size > NVSWITCH_RAW_ERROR_LOG_DATA_SIZE) ||
        (register_block_size > NVSWITCH_RAW_ERROR_LOG_DATA_SIZE))
    {
        return -NVL_BAD_ARGS;
    }

    do
    {
        data->data[i] = NVSWITCH_ENG_OFF_RD32(device, NPORT, , link, register_start);
        register_start += sizeof(NvU32);
        i++;

    }
    while (register_start <= register_end);
    
    *idx = i;
    return NVL_SUCCESS;
}

static void
_nvswitch_collect_error_info_lr10
(
    nvswitch_device    *device,
    NvU32               link,
    NvU32               collect_flags,  // NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_*
    NVSWITCH_RAW_ERROR_LOG_TYPE *data
)
{
    NvU32 val;
    NvU32 i = 0;
    NvBool data_collect_error = NV_FALSE;
    NvlStatus status = NVL_SUCCESS;

    //
    // The requested data 'collect_flags' is captured, if valid.
    // if the error log buffer fills, then the currently captured data block
    // could be truncated and subsequent blocks will be skipped.
    // The 'flags' field in the log structure describes which blocks are
    // actually captured.
    // Captured blocks are packed, in order.
    //

    data->flags = 0;

    // ROUTE
    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME)
    {
        status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                     NV_ROUTE_ERR_TIMESTAMP_LOG,
                     NV_ROUTE_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME;
            NVSWITCH_PRINT(device, INFO,
                "ROUTE: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
        else
        {
            data_collect_error = NV_TRUE;
            NVSWITCH_PRINT(device, ERROR,
                "ROUTE: TIMESTAMP: Error collecting error data\n");
        }
    }

    val = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_HEADER_LOG_VALID);

    if (FLD_TEST_DRF_NUM(_ROUTE, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_ROUTE_ERR_MISC_LOG_0,
                         NV_ROUTE_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "ROUTE: MISC: 0x%08x\n", data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "ROUTE: MISC: Error collecting error data\n");
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_ROUTE_ERR_HEADER_LOG_0,
                         NV_ROUTE_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "ROUTE: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
                    data->data[i-12], data->data[i-11], data->data[i-10], data->data[i-9],
                    data->data[i-8], data->data[i-7], data->data[i-6], data->data[i-5]);
                NVSWITCH_PRINT(device, INFO,
                    "ROUTE: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
                    data->data[i-4], data->data[i-3], data->data[i-2], data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "ROUTE: HEADER: Error collecting error data\n");
            }
        }
    }

    // INGRESS
    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME)
    {
        status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                     NV_INGRESS_ERR_TIMESTAMP_LOG,
                     NV_INGRESS_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME;
            NVSWITCH_PRINT(device, INFO,
                "INGRESS: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
        else
        {
            data_collect_error = NV_TRUE;
            NVSWITCH_PRINT(device, ERROR,
                "INGRESS: TIMESTAMP: Error collecting error data\n");
        }
    }

    val = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_HEADER_LOG_VALID);

    if (FLD_TEST_DRF_NUM(_INGRESS, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_INGRESS_ERR_MISC_LOG_0,
                         NV_INGRESS_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "INGRESS: MISC: 0x%08x\n", data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "INGRESS: MISC: Error collecting error data\n");
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_INGRESS_ERR_HEADER_LOG_0,
                         NV_INGRESS_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "INGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
                    data->data[i-12], data->data[i-11], data->data[i-10], data->data[i-9],
                    data->data[i-8], data->data[i-7], data->data[i-6], data->data[i-5]);
                NVSWITCH_PRINT(device, INFO,
                    "INGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
                    data->data[i-4], data->data[i-3], data->data[i-2], data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "INGRESS: HEADER: Error collecting error data\n");
            }
        }
    }

    // EGRESS
    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME)
    {
        status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                     NV_EGRESS_ERR_TIMESTAMP_LOG,
                     NV_EGRESS_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME;
            NVSWITCH_PRINT(device, INFO,
                "EGRESS: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
        else
        {
            data_collect_error = NV_TRUE;
            NVSWITCH_PRINT(device, ERROR,
                "EGRESS: TIMESTAMP: Error collecting error data\n");
        }
    }

    val = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_HEADER_LOG_VALID);

    if (FLD_TEST_DRF_NUM(_EGRESS, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_EGRESS_ERR_MISC_LOG_0,
                         NV_EGRESS_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: MISC: 0x%08x\n", data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "EGRESS: MISC: Error collecting error data\n");
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR)
        {
            status = _nvswitch_collect_nport_error_info_lr10(device, link, data, &i,
                         NV_EGRESS_ERR_HEADER_LOG_0,
                         NV_EGRESS_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
                    data->data[i-12], data->data[i-11], data->data[i-10], data->data[i-9],
                    data->data[i-8], data->data[i-7], data->data[i-6], data->data[i-5]);
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
                    data->data[i-4], data->data[i-3], data->data[i-2], data->data[i-1]);
            }
            else
            {
                data_collect_error = NV_TRUE;
                NVSWITCH_PRINT(device, ERROR,
                    "EGRESS: HEADER: Error collecting error data\n");
            }
        }
    }

    while (i < NVSWITCH_RAW_ERROR_LOG_DATA_SIZE)
    {
        data->data[i++] = 0;
    }

    if (data_collect_error)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error collecting error info 0x%x.  Only 0x%x error data collected.\n",
            __FUNCTION__, collect_flags, data->flags);
    }
}

static NvlStatus
_nvswitch_service_route_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.route.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_FIRST_0);
    contain = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
        &data);

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _ROUTEBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_ROUTEBUFERR, "route buffer over/underflow", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_ROUTEBUFERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _GLT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE,
                _ERR_GLT_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_ROUTE_ERR_GLT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE,
                                               _ERR_GLT_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, "route GLT DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _TRANSDONERESVERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_TRANSDONERESVERR, "route transdone over/underflow", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_TRANSDONERESVERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _PDCTRLPARERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_PDCTRLPARERR, "route parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_PDCTRLPARERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, "route incoming DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_STATUS_0,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _CDTPARERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_CDTPARERR, "route credit parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_CDTPARERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_CDTPARERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_route_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.route.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_FIRST_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
        &data);

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _NOPORTDEFINEDERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NOPORTDEFINEDERR, "route undefined route");
        NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_NOPORTDEFINEDERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _INVALIDROUTEPOLICYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_INVALIDROUTEPOLICYERR, "route invalid policy");
        NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_INVALIDROUTEPOLICYERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, "route incoming ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_NON_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_STATUS_0, pending);

    //
    // Note, when traffic is flowing, if we reset ERR_COUNT before ERR_STATUS
    // register, we won't see an interrupt again until counter wraps around.
    // In that case, we will miss writing back many ECC victim entries. Hence,
    // always clear _ERR_COUNT only after _ERR_STATUS register is cleared!
    //
    NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER, 0x0);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

//
// Ingress
//

static NvlStatus
_nvswitch_service_ingress_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.ingress.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_FIRST_0);
    contain = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
        &data);

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _CMDDECODEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_CMDDECODEERR, "ingress invalid command", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_CMDDECODEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, "ingress header DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_STATUS_0,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _INVALIDVCSET, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_INVALIDVCSET, "ingress invalid VCSet", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_INVALIDVCSET, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _REMAPTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                _ERR_REMAPTAB_ECC_ERROR_ADDRESS);

        if (FLD_TEST_DRF(_INGRESS_ERR_REMAPTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                                               _ERR_REMAPTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, "ingress Remap DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RIDTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                _ERR_RIDTAB_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_INGRESS_ERR_RIDTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                                               _ERR_RIDTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, "ingress RID DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RLANTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                _ERR_RLANTAB_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_INGRESS_ERR_RLANTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS,
                                               _ERR_RLANTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR, "ingress RLAN DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_NCISOC_PARITY_ERR, "ingress control parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_NCISOC_PARITY_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_PARITY_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_ingress_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.ingress.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_FIRST_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
        &data);

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _REQCONTEXTMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_REQCONTEXTMISMATCHERR, "ingress request context mismatch");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_REQCONTEXTMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _ACLFAIL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_ACLFAIL, "ingress invalid ACL");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_ACLFAIL, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR, "ingress header ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _ADDRBOUNDSERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_ADDRBOUNDSERR, "ingress address bounds");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_ADDRBOUNDSERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RIDTABCFGERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_RIDTABCFGERR, "ingress RID packet");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_RIDTABCFGERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RLANTABCFGERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_RLANTABCFGERR, "ingress RLAN packet");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_RLANTABCFGERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _ADDRTYPEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_ADDRTYPEERR, "ingress illegal address");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_ADDRTYPEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }
    
    NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

//
// Egress
//

static NvlStatus
_nvswitch_service_tstate_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.tstate.nonfatal;
    report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_MISC_LOG_0);
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_FIRST_0);

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                    _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_TSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                                                   _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
                DRF_DEF(_TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR, "TS tag store single-bit threshold");
            _nvswitch_collect_error_info_lr10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
                &data);
            NVSWITCH_REPORT_DATA(_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                    _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_TSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                                                   _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                DRF_DEF(_TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR, "TS crumbstore single-bit threshold");
            _nvswitch_collect_error_info_lr10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
                &data);
            NVSWITCH_REPORT_DATA(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_tstate_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.tstate.fatal;
    report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_MISC_LOG_0);
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_FIRST_0);
    contain = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOLBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_TAGPOOLBUFERR, "TS pointer crossover", NV_FALSE);
        _nvswitch_collect_error_info_lr10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_TAGPOOLBUFERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_TSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                                               _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
            DRF_DEF(_TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, "TS tag store fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_lr10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_STATUS_0,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTOREBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CRUMBSTOREBUFERR, "TS crumbstore", NV_FALSE);
        _nvswitch_collect_error_info_lr10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_CRUMBSTOREBUFERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_TSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE,
                                               _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
            DRF_DEF(_TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, "TS crumbstore fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_lr10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_STATUS_0,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _ATO_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (FLD_TEST_DRF_NUM(_TSTATE, _ERR_FIRST_0, _ATO_ERR, 1, report.raw_first))
        {
            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _TSTATE, _ERR_DEBUG);
        }
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_ATO_ERR, "TS ATO timeout", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CAMRSP_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CAMRSP_ERR, "Rsp Tag value out of range", NV_FALSE);
        _nvswitch_collect_error_info_lr10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_CAMRSP_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_egress_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.egress.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_FIRST_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
        &data);

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, "egress input ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS,
                    _ERR_RAM_OUT_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_EGRESS_ERR_RAM_OUT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS,
                                                   _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER);
            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, "egress output ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, link, bAddressValid, address,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _URRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_DROPNPURRSPERR, "egress non-posted UR");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_DROPNPURRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _PRIVRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_PRIVRSPERR, "egress non-posted PRIV error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_PRIVRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _HWRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_HWRSPERR, "egress non-posted HW error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_HWRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_egress_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    NVSWITCH_RAW_ERROR_LOG_TYPE credit_data = {0, { 0 }};
    NVSWITCH_RAW_ERROR_LOG_TYPE buffer_data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.egress.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_FIRST_0);
    contain = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_lr10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
        &data);

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _EGRESSBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_EGRESSBUFERR, "egress crossbar overflow", NV_TRUE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_EGRESSBUFERR, data);

        buffer_data.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS0);
        buffer_data.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS1);
        buffer_data.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS2);
        buffer_data.data[3] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS3);
        buffer_data.data[4] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS4);
        buffer_data.data[5] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS5);
        buffer_data.data[6] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS6);
        buffer_data.data[7] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _BUFFER_POINTERS7);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_EGRESSBUFERR, buffer_data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _PKTROUTEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_PKTROUTEERR, "egress packet route", NV_TRUE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_PKTROUTEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _SEQIDERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_SEQIDERR, "egress sequence ID error", NV_TRUE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_SEQIDERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_DBE_ERR, "egress input ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_STATUS_0,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS,
                _ERR_RAM_OUT_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_EGRESS_ERR_RAM_OUT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS,
                                               _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, "egress output ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_STATUS_0,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NCISOCCREDITOVFL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NCISOCCREDITOVFL, "egress credit overflow", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOCCREDITOVFL, data);

        credit_data.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT0);
        credit_data.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT1);
        credit_data.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT2);
        credit_data.data[3] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT3);
        credit_data.data[4] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT4);
        credit_data.data[5] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT5);
        credit_data.data[6] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT6);
        credit_data.data[7] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT7);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOCCREDITOVFL, credit_data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _REQTGTIDMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_REQTGTIDMISMATCHERR, "egress destination request ID error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_REQTGTIDMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _RSPREQIDMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_RSPREQIDMISMATCHERR, "egress destination response ID error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_RSPREQIDMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, "egress control parity error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NCISOC_CREDIT_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, "egress credit parity error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, data);

        credit_data.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT0);
        credit_data.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT1);
        credit_data.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT2);
        credit_data.data[3] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT3);
        credit_data.data[4] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT4);
        credit_data.data[5] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT5);
        credit_data.data[6] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT6);
        credit_data.data[7] = NVSWITCH_NPORT_RD32_LR10(device, link, _EGRESS, _NCISOC_CREDIT7);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, credit_data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_FLITTYPE_MISMATCH_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NXBAR_FLITTYPE_MISMATCH_ERR, "egress flit type mismatch", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NXBAR_FLITTYPE_MISMATCH_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _CREDIT_TIME_OUT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_CREDIT_TIME_OUT_ERR, "egress credit timeout", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_CREDIT_TIME_OUT_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_sourcetrack_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32           link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link,
                            _SOURCETRACK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link,
                            _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.sourcetrack.nonfatal;

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK, _ERR_FIRST_0);

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                    _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                             _VALID, _VALID, addressValid))
            {
                address = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                                   _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER);
            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
            report.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR,
                                    "sourcetrack TCEN0 crumbstore ECC limit err");

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR, 1))))
            {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                    _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN1_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                             _VALID, _VALID, addressValid))
            {
                address = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                                   _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_COUNTER);
            report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS);
            report.data[2] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR,
                                    "sourcetrack TCEN1 crumbstore ECC limit err");

            _nvswitch_construct_ecc_error_event(&err_event,
                NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    //
    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    //
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_sourcetrack_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_NPORT_RD32_LR10(device, link,
                            _SOURCETRACK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NPORT_RD32_LR10(device, link,
                            _SOURCETRACK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.sourcetrack.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK, _ERR_FIRST_0);
    contain = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                         _VALID, _VALID, addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                               _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                            _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                            _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR,
                                "sourcetrack TCEN0 crumbstore DBE", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR,
            link, bAddressValid, address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_STATUS_0,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN1_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                         _VALID, _VALID, addressValid))
        {
            address = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                                               _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                            _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS);
        report.data[1] = NVSWITCH_NPORT_RD32_LR10(device, link, _SOURCETRACK,
                            _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR,
                                "sourcetrack TCEN1 crumbstore DBE", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event(&err_event,
            NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN1_CRUMBSTORE_ECC_DBE_ERR,
            link, bAddressValid, address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_STATUS_0,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN1_CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _SOURCETRACK_TIME_OUT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_SOURCETRACK_TIME_OUT_ERR,
                                "sourcetrack timeout error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    //
    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    //
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;

}

static NvlStatus
_nvswitch_service_nport_fatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    NvlStatus status[5];

    status[0] = _nvswitch_service_route_fatal_lr10(device, link);
    status[1] = _nvswitch_service_ingress_fatal_lr10(device, link);
    status[2] = _nvswitch_service_egress_fatal_lr10(device, link);
    status[3] = _nvswitch_service_tstate_fatal_lr10(device, link);
    status[4] = _nvswitch_service_sourcetrack_fatal_lr10(device, link);

    if ((status[0] != NVL_SUCCESS) &&
        (status[1] != NVL_SUCCESS) &&
        (status[2] != NVL_SUCCESS) &&
        (status[3] != NVL_SUCCESS) &&
        (status[4] != NVL_SUCCESS))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_npg_fatal_lr10
(
    nvswitch_device *device,
    NvU32            npg
)
{
    NvU32 pending, mask, bit, unhandled;
    NvU32 nport;
    NvU32 link;

    pending = NVSWITCH_NPG_RD32_LR10(device, npg, _NPG, _NPG_INTERRUPT_STATUS);
 
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    mask = 
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV0_INT_STATUS, _FATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV1_INT_STATUS, _FATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV2_INT_STATUS, _FATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV3_INT_STATUS, _FATAL);
    pending &= mask;
    unhandled = pending;

    for (nport = 0; nport < NVSWITCH_NPORT_PER_NPG; nport++)
    {
        switch (nport)
        {
            case 0:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV0_INT_STATUS, _FATAL);
                break;
            case 1:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV1_INT_STATUS, _FATAL);
                break;
            case 2:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV2_INT_STATUS, _FATAL);
                break;
            case 3:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV3_INT_STATUS, _FATAL);
                break;
            default:
                bit = 0;
                NVSWITCH_ASSERT(0);
                break;
        }
        if (nvswitch_test_flags(pending, bit))
        {
            link = NPORT_TO_LINK(device, npg, nport);
            if (NVSWITCH_ENG_VALID_LR10(device, NPORT, link))
            {
                if (_nvswitch_service_nport_fatal_lr10(device, link) == NVL_SUCCESS)
                {
                    nvswitch_clear_flags(&unhandled, bit);
                }
            }
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nport_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32            link
)
{
    NvlStatus status[5];

    status[0] = _nvswitch_service_route_nonfatal_lr10(device, link);
    status[1] = _nvswitch_service_ingress_nonfatal_lr10(device, link);
    status[2] = _nvswitch_service_egress_nonfatal_lr10(device, link);
    status[3] = _nvswitch_service_tstate_nonfatal_lr10(device, link);
    status[4] = _nvswitch_service_sourcetrack_nonfatal_lr10(device, link);

    if ((status[0] != NVL_SUCCESS) &&
        (status[1] != NVL_SUCCESS) &&
        (status[2] != NVL_SUCCESS) &&
        (status[3] != NVL_SUCCESS) &&
        (status[4] != NVL_SUCCESS))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_npg_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 npg
)
{
    NvU32 pending, mask, bit, unhandled;
    NvU32 nport;
    NvU32 link;

    pending = NVSWITCH_NPG_RD32_LR10(device, npg, _NPG, _NPG_INTERRUPT_STATUS);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    mask = 
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV0_INT_STATUS, _NONFATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV1_INT_STATUS, _NONFATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV2_INT_STATUS, _NONFATAL) |
        DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV3_INT_STATUS, _NONFATAL);
    pending &= mask;
    unhandled = pending;

    for (nport = 0; nport < NVSWITCH_NPORT_PER_NPG; nport++)
    {
        switch (nport)
        {
            case 0:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV0_INT_STATUS, _NONFATAL);
                break;
            case 1:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV1_INT_STATUS, _NONFATAL);
                break;
            case 2:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV2_INT_STATUS, _NONFATAL);
                break;
            case 3:
                bit = DRF_DEF(_NPG, _NPG_INTERRUPT_STATUS, _DEV3_INT_STATUS, _NONFATAL);
                break;
            default:
                bit = 0;
                NVSWITCH_ASSERT(0);
                break;
        }
        if (nvswitch_test_flags(pending, bit))
        {
            link = NPORT_TO_LINK(device, npg, nport);
            if (NVSWITCH_ENG_VALID_LR10(device, NPORT, link))
            {
                if (_nvswitch_service_nport_nonfatal_lr10(device, link) == NVL_SUCCESS)
                {
                    nvswitch_clear_flags(&unhandled, bit);
                }
            }
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_service_minion_link_lr10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, unhandled, minionIntr, linkIntr, reg, enabledLinks, bit;
    NvU32 localLinkIdx, link;

    //
    // _MINION_MINION_INTR shows all interrupts currently at the host on this minion
    // Note: _MINIO_MINION_INTR is not used to clear link specific interrupts
    //
    minionIntr = NVSWITCH_MINION_RD32_LR10(device, instance, _MINION, _MINION_INTR);

    // get all possible interrupting links associated with this minion
    report.raw_pending = DRF_VAL(_MINION, _MINION_INTR, _LINK, minionIntr);

    // read in the enaled minion interrupts on this minion
    reg = NVSWITCH_MINION_RD32_LR10(device, instance, _MINION, _MINION_INTR_STALL_EN);

    // get the links with enabled interrupts on this minion
    enabledLinks = DRF_VAL(_MINION, _MINION_INTR_STALL_EN, _LINK, reg);

    report.raw_enable = enabledLinks;
    report.mask = report.raw_enable;

    // pending bit field contains interrupting links after being filtered
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    FOR_EACH_INDEX_IN_MASK(32, localLinkIdx, pending)
    {
        link = (instance * NVSWITCH_LINKS_PER_NVLIPT) + localLinkIdx;
        bit = NVBIT(localLinkIdx);

        // read in the interrupt register for the given link
        linkIntr = NVSWITCH_MINION_LINK_RD32_LR10(device, link, _MINION, _NVLINK_LINK_INTR(localLinkIdx));

        // _STATE must be set for _CODE to be valid
        if (!DRF_VAL(_MINION, _NVLINK_LINK_INTR, _STATE, linkIntr))
        {
            continue;
        }

        report.data[0] = linkIntr;

        switch(DRF_VAL(_MINION, _NVLINK_LINK_INTR, _CODE, linkIntr))
        {
            case NV_MINION_NVLINK_LINK_INTR_CODE_NA:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link NA interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_SWREQ:
                NVSWITCH_PRINT(device, INFO,
                      "%s: Received MINION Link SW Generate interrupt on MINION %d : link %d.\n",
                      __FUNCTION__, instance, link);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_DLREQ:
                NVSWITCH_REPORT_NONFATAL(_HW_MINION_NONFATAL, "Minion Link DLREQ interrupt");
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_PMDISABLED:
                NVSWITCH_REPORT_NONFATAL(_HW_MINION_NONFATAL, "Minion Link PMDISABLED interrupt");
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_DLCMDFAULT:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link DLCMDFAULT interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_TLREQ:
                NVSWITCH_REPORT_NONFATAL(_HW_MINION_NONFATAL, "Minion Link TLREQ interrupt");
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_NOINIT:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link NOINIT interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_NOTIFY:
                NVSWITCH_PRINT(device, INFO,
                      "%s: Received MINION NOTIFY interrupt on MINION %d : link %d.\n",
                      __FUNCTION__, instance, link);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_LOCAL_CONFIG_ERR:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link Local-Config-Error interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_NEGOTIATION_CONFIG_ERR:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link Negotiation Config Err Interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_BADINIT: 
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link BADINIT interrupt", NV_FALSE);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_PMFAIL:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link PMFAIL interrupt", NV_FALSE);
                break;
            default:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Interrupt code unknown", NV_FALSE);
        }
        nvswitch_clear_flags(&unhandled, bit);

        // Disable interrupt bit for the given link - fatal error ocurred before
        if (device->link[link].fatal_error_occurred)
        {
            enabledLinks &= ~bit;
            reg = DRF_NUM(_MINION, _MINION_INTR_STALL_EN, _LINK, enabledLinks);
            NVSWITCH_MINION_LINK_WR32_LR10(device, link, _MINION, _MINION_INTR_STALL_EN, reg);
        }

        //
        // _MINION_INTR_LINK is a read-only register field for the host
        // Host must write 1 to _NVLINK_LINK_INTR_STATE to clear the interrupt on the link
        //
        reg = DRF_NUM(_MINION, _NVLINK_LINK_INTR, _STATE, 1);
        NVSWITCH_MINION_WR32_LR10(device, instance, _MINION, _NVLINK_LINK_INTR(localLinkIdx), reg);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvldl_nonfatal_link_lr10
(
    nvswitch_device *device,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_REPLAY, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_TX_REPLAY, "TX Replay Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_RECOVERY_SHORT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_TX_RECOVERY_SHORT, "TX Recovery Short");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_SHORT_ERROR_RATE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_SHORT_ERROR_RATE, "RX Short Error Rate");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_LONG_ERROR_RATE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_LONG_ERROR_RATE, "RX Long Error Rate");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_ILA_TRIGGER, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_ILA_TRIGGER, "RX ILA Trigger");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_CRC_COUNTER, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_CRC_COUNTER, "RX CRC Counter");
        nvswitch_clear_flags(&unhandled, bit);

        //
        // Mask CRC counter after first occurrance - otherwise, this interrupt
        // will continue to fire once the CRC counter has hit the threshold
        // See Bug 3341528
        //
        report.raw_enable = report.raw_enable & (~bit);
        NVSWITCH_LINK_WR32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN,
            report.raw_enable);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    NVSWITCH_LINK_WR32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLDL nonfatal interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvldl_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            // An interrupt on an invalid link should never occur
            NVSWITCH_ASSERT(link != NULL);
            continue;
        }

        if (NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT) != nvlipt_instance)
        {
            NVSWITCH_ASSERT(0);
            break;
        }

        if (nvswitch_is_link_in_reset(device, link))
        {
            continue;
        }

        if (_nvswitch_service_nvldl_nonfatal_link_lr10(device, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_lnk_nonfatal_0_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXRSPSTATUS_PRIV_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_RX_LNK_RXRSPSTATUS_PRIV_ERR, "RX Rsp Status PRIV Error");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_LNK, _ERR_REPORT_INJECT_0, _RXRSPSTATUS_PRIV_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSP_STATUS_PRIV_ERR_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_RX_LNK _0 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_tx_lnk_nonfatal_0_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _CREQ_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_CREQ_RAM_DAT_ECC_DBE_ERR, "CREQ RAM DAT ECC DBE Error");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_TX_LNK, _ERR_REPORT_INJECT_0, _CREQ_RAM_DAT_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_TX_CREQ_DAT_RAM_ECC_DBE_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _CREQ_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_CREQ_RAM_ECC_LIMIT_ERR, "CREQ RAM DAT ECC Limit Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_RSP_RAM_DAT_ECC_DBE_ERR, "Response RAM DAT ECC DBE Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_RSP_RAM_ECC_LIMIT_ERR, "Response RAM ECC Limit Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _COM_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_COM_RAM_DAT_ECC_DBE_ERR, "COM RAM DAT ECC DBE Error");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_TX_LNK, _ERR_REPORT_INJECT_0, _COM_RAM_DAT_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_TX_COM_DAT_RAM_ECC_DBE_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _COM_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_COM_RAM_ECC_LIMIT_ERR, "COM RAM ECC Limit Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP1_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_RSP1_RAM_ECC_LIMIT_ERR, "RSP1 RAM ECC Limit Error");
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_TX_LNK _0 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_lnk_nonfatal_1_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _AN1_HEARTBEAT_TIMEOUT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_RX_LNK_AN1_HEARTBEAT_TIMEOUT_ERR, "AN1 Heartbeat Timeout Error");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_1, _AN1_HEARTBEAT_TIMEOUT_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_AN1_HEARTBEAT_TIMEOUT_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
            //
            // WAR Bug 200627368: Mask off HBTO to avoid a storm
            // During the start of reset_and_drain, all links on the GPU
            // will go into contain, causing HBTO on other switch links connected
            // to that GPU. For the switch side, these interrupts are not fatal,
            // but until we get to reset_and_drain for this link, HBTO will continue
            // to fire repeatedly. After reset_and_drain, HBTO will be re-enabled
            // by MINION after links are trained.
            //
            report.raw_enable = report.raw_enable & (~bit);
            NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
                report.raw_enable);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
            report.raw_enable & (~pending));
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_RX_LNK _1 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_tx_lnk_nonfatal_1_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_1);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_REPORT_INJECT_1);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC0, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC0, "AN1 Timeout VC0");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC0, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC0_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC1, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC1, "AN1 Timeout VC1");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC1, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC1_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC2, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC2, "AN1 Timeout VC2");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC2, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC2_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC3, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC3, "AN1 Timeout VC3");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC3, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC3_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC4, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC4, "AN1 Timeout VC4");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC4, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC4_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC5, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC5, "AN1 Timeout VC5");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC5, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC5_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC6, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC6, "AN1 Timeout VC6");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC6, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC6_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _AN1_TIMEOUT_VC7, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC7, "AN1 Timeout VC7");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_1, _AN1_TIMEOUT_VC7, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC7_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_1, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_TX_LNK _1 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            // An interrupt on an invalid link should never occur
            NVSWITCH_ASSERT(link != NULL);
            continue;
        }

        if (NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT) != nvlipt_instance)
        {
            NVSWITCH_ASSERT(0);
            break;
        }

        if (nvswitch_is_link_in_reset(device, link))
        {
            continue;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_nonfatal_0_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_tx_lnk_nonfatal_0_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_nonfatal_1_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_tx_lnk_nonfatal_1_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

static NvlStatus
_nvswitch_service_nvlipt_lnk_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _ILLEGALLINKSTATEREQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_ILLEGALLINKSTATEREQUEST, "_HW_NVLIPT_LNK_ILLEGALLINKSTATEREQUEST");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _ILLEGALLINKSTATEREQUEST, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_ILLEGAL_LINK_STATE_REQUEST_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _FAILEDMINIONREQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_FAILEDMINIONREQUEST, "_FAILEDMINIONREQUEST");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _FAILEDMINIONREQUEST, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_FAILED_MINION_REQUEST_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _RESERVEDREQUESTVALUE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_RESERVEDREQUESTVALUE, "_RESERVEDREQUESTVALUE");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _RESERVEDREQUESTVALUE, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RESERVED_REQUEST_VALUE_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _LINKSTATEWRITEWHILEBUSY, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_LINKSTATEWRITEWHILEBUSY, "_LINKSTATEWRITEWHILEBUSY");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _LINKSTATEWRITEWHILEBUSY, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_LINK_STATE_WRITE_WHILE_BUSY_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _LINK_STATE_REQUEST_TIMEOUT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_LINK_STATE_REQUEST_TIMEOUT, "_LINK_STATE_REQUEST_TIMEOUT");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _LINK_STATE_REQUEST_TIMEOUT, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_LINK_STATE_REQUEST_TIMEOUT_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _WRITE_TO_LOCKED_SYSTEM_REG_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_WRITE_TO_LOCKED_SYSTEM_REG_ERR, "_WRITE_TO_LOCKED_SYSTEM_REG_ERR");
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _WRITE_TO_LOCKED_SYSTEM_REG_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLIPT_LNK NON_FATAL interrupts, pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlipt_link_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU32 i, intrLink;
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask, interruptingLinks = 0;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        intrLink = NVSWITCH_LINK_RD32_LR10(device, i, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);

        if(intrLink)
        {
            interruptingLinks |= NVBIT(i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if(interruptingLinks)
    {
        FOR_EACH_INDEX_IN_MASK(64, i, interruptingLinks)
        {
            if( _nvswitch_service_nvlipt_lnk_nonfatal_lr10(device, nvlipt_instance, i) != NVL_SUCCESS)
            {
                return -NVL_MORE_PROCESSING_REQUIRED;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
        return NVL_SUCCESS;
    }
    else
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }
}

static NvlStatus
_nvswitch_service_nvlipt_nonfatal_lr10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvlStatus status[4];

    //
    // MINION LINK interrupts trigger both INTR_FATAL and INTR_NONFATAL
    // trees (Bug 3037835). Because of this, we must service them in both the
    // fatal and nonfatal handlers
    //
    status[0] = device->hal.nvswitch_service_minion_link(device, instance);
    status[1] = _nvswitch_service_nvldl_nonfatal_lr10(device, instance);
    status[2] = _nvswitch_service_nvltlc_nonfatal_lr10(device, instance);
    status[3] = _nvswitch_service_nvlipt_link_nonfatal_lr10(device, instance);

    if (status[0] != NVL_SUCCESS &&
        status[1] != NVL_SUCCESS &&
        status[2] != NVL_SUCCESS &&
        status[3] != NVL_SUCCESS)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_soe_fatal_lr10
(
    nvswitch_device *device
)
{
    // We only support 1 SOE as of LR10.
    if (soeService_HAL(device, (PSOE)device->pSoe) != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_saw_legacy_lr10
(
    nvswitch_device *device
)
{
    //TODO : SAW Legacy interrupts

    return -NVL_MORE_PROCESSING_REQUIRED;
}

static NvlStatus
_nvswitch_service_saw_nonfatal_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 pending, bit, unhandled;
    NvU32 i;

    pending = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_NONFATAL);
    pending &= chip_device->intr_enable_nonfatal;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    for (i = 0; i < NUM_NPG_ENGINE_LR10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, NPG, i))
            continue;

        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_NONFATAL, _NPG_0, 1) << i;
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_npg_nonfatal_lr10(device, i) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    for (i = 0; i < NUM_NVLIPT_ENGINE_LR10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, NVLIPT, i))
        {
            continue;
        }

        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_NONFATAL, _NVLIPT_0, 1) << i;

        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_nvlipt_nonfatal_lr10(device, i) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nxbar_tile_lr10
(
    nvswitch_device *device,
    NvU32 link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };

    report.raw_pending = NVSWITCH_TILE_RD32_LR10(device, link, _NXBAR_TILE, _ERR_STATUS);
    report.raw_enable = NVSWITCH_TILE_RD32_LR10(device, link, _NXBAR_TILE, _ERR_FATAL_INTR_EN);
    report.mask = chip_device->intr_mask.tile.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_TILE_RD32_LR10(device, link, _NXBAR_TILE, _ERR_FIRST);

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_BUFFER_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_BUFFER_OVERFLOW, "ingress SRC-VC buffer overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_BUFFER_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_BUFFER_UNDERFLOW, "ingress SRC-VC buffer underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _EGRESS_CREDIT_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_EGRESS_CREDIT_OVERFLOW, "egress DST-VC credit overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _EGRESS_CREDIT_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_EGRESS_CREDIT_UNDERFLOW, "egress DST-VC credit underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_NON_BURSTY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_NON_BURSTY_PKT, "ingress packet burst error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_NON_STICKY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_NON_STICKY_PKT, "ingress packet sticky error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_BURST_GT_9_DATA_VC, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_BURST_GT_9_DATA_VC, "possible bubbles at ingress", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_PKT_INVALID_DST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_PKT_INVALID_DST, "ingress packet invalid dst error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILE, _ERR_STATUS, _INGRESS_PKT_PARITY_ERROR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILE_INGRESS_PKT_PARITY_ERROR, "ingress packet parity error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_TILE_WR32_LR10(device, link, _NXBAR_TILE, _ERR_FIRST,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    NVSWITCH_TILE_WR32_LR10(device, link, _NXBAR_TILE, _ERR_FATAL_INTR_EN,
                            report.raw_enable ^ pending);

    NVSWITCH_TILE_WR32_LR10(device, link, _NXBAR_TILE, _ERR_STATUS, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nxbar_tileout_lr10
(
    nvswitch_device *device,
    NvU32 link,
    NvU32 tileout
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };

    report.raw_pending = NVSWITCH_NXBAR_RD32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_STATUS(tileout));
    report.raw_enable = NVSWITCH_NXBAR_RD32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_FATAL_INTR_EN(tileout));
    report.mask = chip_device->intr_mask.tileout.fatal;
    report.data[0] = tileout;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_NXBAR_RD32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_FIRST(tileout));

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _INGRESS_BUFFER_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BUFFER_OVERFLOW, "ingress SRC-VC buffer overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _INGRESS_BUFFER_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BUFFER_UNDERFLOW, "ingress SRC-VC buffer underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _EGRESS_CREDIT_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CREDIT_OVERFLOW, "egress DST-VC credit overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _EGRESS_CREDIT_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CREDIT_UNDERFLOW, "egress DST-VC credit underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _INGRESS_NON_BURSTY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_NON_BURSTY_PKT, "ingress packet burst error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _INGRESS_NON_STICKY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_NON_STICKY_PKT, "ingress packet sticky error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _INGRESS_BURST_GT_9_DATA_VC, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BURST_GT_9_DATA_VC, "possible bubbles at ingress", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR, _TC_TILEOUT0_ERR_STATUS, _EGRESS_CDT_PARITY_ERROR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CDT_PARITY_ERROR, "ingress credit parity error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_NXBAR_WR32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_FIRST(tileout),
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    NVSWITCH_NXBAR_WR32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_FATAL_INTR_EN(tileout),
                            report.raw_enable ^ pending);

    NVSWITCH_NXBAR_WR32_LR10(device, link, _NXBAR_TC_TILEOUT, _ERR_STATUS(tileout), pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nxbar_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nxbar
)
{
    NvU32 pending, bit, unhandled;
    NvU32 link;
    NvU32 tile, tileout;

    pending = NVSWITCH_NXBAR_RD32_LR10(device, nxbar, _NXBAR, _TC_ERROR_STATUS);
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    for (tile = 0; tile < NUM_NXBAR_TILES_PER_TC_LR10; tile++)
    {
        bit = DRF_NUM(_NXBAR, _TC_ERROR_STATUS, _TILE0, 1) << tile;
        if (nvswitch_test_flags(pending, bit))
        {
            link = TILE_TO_LINK(device, nxbar, tile);
            if (NVSWITCH_ENG_VALID_LR10(device, TILE, link))
            {
                if (_nvswitch_service_nxbar_tile_lr10(device, link) == NVL_SUCCESS)
                {
                    nvswitch_clear_flags(&unhandled, bit);
                }
            }
        }
    }

    for (tileout = 0; tileout < NUM_NXBAR_TILEOUTS_PER_TC_LR10; tileout++)
    {
        bit = DRF_NUM(_NXBAR, _TC_ERROR_STATUS, _TILEOUT0, 1) << tileout;
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_nxbar_tileout_lr10(device, nxbar, tileout) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    // TODO: Perform hot_reset to recover NXBAR

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);


    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

NvlStatus
_nvswitch_service_minion_fatal_lr10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvU32 pending, bit, unhandled, mask;

    pending = NVSWITCH_MINION_RD32_LR10(device, instance, _MINION, _MINION_INTR);
    mask =  NVSWITCH_MINION_RD32_LR10(device, instance, _MINION, _MINION_INTR_STALL_EN);

    // Don't consider MINION Link interrupts in this handler
    mask &= ~(DRF_NUM(_MINION, _MINION_INTR_STALL_EN, _LINK, NV_MINION_MINION_INTR_STALL_EN_LINK_ENABLE_ALL));

    pending &= mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending; 

    bit = DRF_NUM(_MINION, _MINION_INTR, _FALCON_STALL, 0x1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (nvswitch_minion_service_falcon_interrupts_lr10(device, instance) == NVL_SUCCESS)
        {
            nvswitch_clear_flags(&unhandled, bit);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_service_nvldl_fatal_link_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_FAULT_RAM, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_TX_FAULT_RAM, "TX Fault Ram", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_TX_FAULT_RAM_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_FAULT_INTERFACE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_TX_FAULT_INTERFACE, "TX Fault Interface", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_TX_FAULT_INTERFACE_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_FAULT_SUBLINK_CHANGE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_TX_FAULT_SUBLINK_CHANGE, "TX Fault Sublink Change", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_TX_FAULT_SUBLINK_CHANGE_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_FAULT_SUBLINK_CHANGE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_RX_FAULT_SUBLINK_CHANGE, "RX Fault Sublink Change", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_RX_FAULT_SUBLINK_CHANGE_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_FAULT_DL_PROTOCOL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_RX_FAULT_DL_PROTOCOL, "RX Fault DL Protocol", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_RX_FAULT_DL_PROTOCOL_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_DOWN, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_FAULT_DOWN, "LTSSM Fault Down", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_LTSSM_FAULT_DOWN_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_UP, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_FAULT_UP, "LTSSM Fault Up", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_LTSSM_FAULT_UP_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_PROTOCOL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_PROTOCOL, "LTSSM Protocol Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        // TODO 2827793 this should be logged to the InfoROM as fatal
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN,
                report.raw_enable ^ pending);
    }

    NVSWITCH_LINK_WR32_LR10(device, link, NVLDL, _NVLDL_TOP, _INTR, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLDL fatal interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

NvlStatus
_nvswitch_service_nvldl_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask, runtimeErrorMask = 0;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;
    NVSWITCH_LINK_TRAINING_ERROR_INFO linkTrainingErrorInfo = { 0 };
    NVSWITCH_LINK_RUNTIME_ERROR_INFO linkRuntimeErrorInfo = { 0 };

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            // An interrupt on an invalid link should never occur
            NVSWITCH_ASSERT(link != NULL);
            continue;
        }

        if (NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT) != nvlipt_instance)
        {
            NVSWITCH_ASSERT(0);
            break;
        }

        if (nvswitch_is_link_in_reset(device, link))
        {
            continue;
        }

        if (device->hal.nvswitch_service_nvldl_fatal_link(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            runtimeErrorMask |= NVBIT64(i);
            status = NVL_SUCCESS;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    linkTrainingErrorInfo.isValid = NV_FALSE;
    linkRuntimeErrorInfo.isValid  = NV_TRUE;
    linkRuntimeErrorInfo.mask0    = runtimeErrorMask;

    if (nvswitch_smbpbi_set_link_error_info(device, &linkTrainingErrorInfo, &linkRuntimeErrorInfo) !=
        NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                       "%s: Unable to send Runtime Error bitmask: 0x%llx,\n",
                       __FUNCTION__, runtimeErrorMask);
    }

    return status;
}

static NvlStatus
_nvswitch_service_nvltlc_tx_sys_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _NCISOC_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_NCISOC_PARITY_ERR, "NCISOC Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_SYS_ERR_REPORT_INJECT_0, _NCISOC_PARITY_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_NCISOC_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _NCISOC_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_NCISOC_HDR_ECC_DBE_ERR, "NCISOC HDR ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_SYS_ERR_REPORT_INJECT_0, _NCISOC_HDR_ECC_DBE_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_NCISOC_HDR_ECC_DBE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _NCISOC_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_NCISOC_DAT_ECC_DBE_ERR, "NCISOC DAT ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _NCISOC_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_NCISOC_ECC_LIMIT_ERR, "NCISOC ECC Limit Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _TXPOISONDET, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TXPOISONDET, "Poison Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _TXRSPSTATUS_HW_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_TXRSPSTATUS_HW_ERR, "TX Response Status HW Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _TXRSPSTATUS_UR_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_TXRSPSTATUS_UR_ERR, "TX Response Status UR Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _TXRSPSTATUS_PRIV_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_TXRSPSTATUS_PRIV_ERR, "TX Response Status PRIV Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_TX_SYS interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_sys_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _NCISOC_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_NCISOC_PARITY_ERR, "NCISOC Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _HDR_RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_HDR_RAM_ECC_DBE_ERR, "HDR RAM ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_SYS, _ERR_REPORT_INJECT_0, _HDR_RAM_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_RX_HDR_RAM_ECC_DBE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _HDR_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_HDR_RAM_ECC_LIMIT_ERR, "HDR RAM ECC Limit Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT0_RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT0_RAM_ECC_DBE_ERR, "DAT0 RAM ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_SYS, _ERR_REPORT_INJECT_0, _DAT0_RAM_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_RX_DAT0_RAM_ECC_DBE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT0_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT0_RAM_ECC_LIMIT_ERR, "DAT0 RAM ECC Limit Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT1_RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT1_RAM_ECC_DBE_ERR, "DAT1 RAM ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_SYS, _ERR_REPORT_INJECT_0, _DAT1_RAM_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_RX_DAT1_RAM_ECC_DBE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT1_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT1_RAM_ECC_LIMIT_ERR, "DAT1 RAM ECC Limit Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_RX_SYS interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_tx_lnk_fatal_0_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _TXDLCREDITPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TXDLCREDITPARITYERR, "TX DL Credit Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _TX_LNK_ERR_REPORT_INJECT_0, _TXDLCREDITPARITYERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_DL_CREDIT_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _CREQ_RAM_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_LNK_CREQ_RAM_HDR_ECC_DBE_ERR, "CREQ RAM HDR ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP_RAM_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_LNK_RSP_RAM_HDR_ECC_DBE_ERR, "Response RAM HDR ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _COM_RAM_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_LNK_COM_RAM_HDR_ECC_DBE_ERR, "COM RAM HDR ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP1_RAM_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_LNK_RSP1_RAM_HDR_ECC_DBE_ERR, "RSP1 RAM HDR ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP1_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_LNK_RSP1_RAM_DAT_ECC_DBE_ERR, "RSP1 RAM DAT ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC_TX_LNK, _ERR_REPORT_INJECT_0, _RSP1_RAM_DAT_ECC_DBE_ERR, 0x0, injected))
        {
            // TODO 3014908 log these in the NVL object until we have ECC object support
            error_event.error = INFOROM_NVLINK_TLC_TX_RSP1_DAT_RAM_ECC_DBE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_TX_LNK _0 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_lnk_fatal_0_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXDLHDRPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXDLHDRPARITYERR, "RX DL HDR Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXDLHDRPARITYERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DL_HDR_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXDLDATAPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXDLDATAPARITYERR, "RX DL Data Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXDLDATAPARITYERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DL_DATA_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXDLCTRLPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXDLCTRLPARITYERR, "RX DL Ctrl Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXDLCTRLPARITYERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DL_CTRL_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXINVALIDAEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXINVALIDAEERR, "RX Invalid DAE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXINVALIDAEERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_AE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXINVALIDBEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXINVALIDBEERR, "RX Invalid BE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXINVALIDBEERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_BE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXINVALIDADDRALIGNERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXINVALIDADDRALIGNERR, "RX Invalid Addr Align Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXINVALIDADDRALIGNERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_ADDR_ALIGN_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXPKTLENERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXPKTLENERR, "RX Packet Length Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXPKTLENERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_PKTLEN_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVCMDENCERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVCMDENCERR, "RSV Cmd Encoding Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RSVCMDENCERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSVD_CMD_ENC_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVDATLENENCERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVDATLENENCERR, "RSV Data Length Encoding Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RSVDATLENENCERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSVD_DAT_LEN_ENC_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVPKTSTATUSERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVPKTSTATUSERR, "RSV Packet Status Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RSVPKTSTATUSERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSVD_PACKET_STATUS_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVCACHEATTRPROBEREQERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVCACHEATTRPROBEREQERR, "RSV Packet Status Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RSVCACHEATTRPROBEREQERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSVD_CACHE_ATTR_PROBE_REQ_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVCACHEATTRPROBERSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVCACHEATTRPROBERSPERR, "RSV CacheAttr Probe Rsp Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RSVCACHEATTRPROBERSPERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSVD_CACHE_ATTR_PROBE_RSP_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _DATLENGTRMWREQMAXERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_DATLENGTRMWREQMAXERR, "Data Length RMW Req Max Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _DATLENGTRMWREQMAXERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DATLEN_GT_RMW_REQ_MAX_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _DATLENLTATRRSPMINERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_DATLENLTATRRSPMINERR, "Data Len Lt ATR RSP Min Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _DATLENLTATRRSPMINERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DATLEN_LT_ATR_RSP_MIN_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _INVALIDCACHEATTRPOERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_INVALIDCACHEATTRPOERR, "Invalid Cache Attr PO Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _INVALIDCACHEATTRPOERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_PO_FOR_CACHE_ATTR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _INVALIDCRERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_INVALIDCRERR, "Invalid CR Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _INVALIDCRERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_CR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXRSPSTATUS_HW_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_LNK_RXRSPSTATUS_HW_ERR, "RX Rsp Status HW Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        // TODO 200564153 _RX_RSPSTATUS_HW_ERR should be reported as non-fatal
        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXRSPSTATUS_HW_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSP_STATUS_HW_ERR_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXRSPSTATUS_UR_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_LNK_RXRSPSTATUS_UR_ERR, "RX Rsp Status UR Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        // TODO 200564153 _RX_RSPSTATUS_UR_ERR should be reported as non-fatal
        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _RXRSPSTATUS_UR_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSP_STATUS_UR_ERR_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _INVALID_COLLAPSED_RESPONSE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_LNK_INVALID_COLLAPSED_RESPONSE_ERR, "Invalid Collapsed Response Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_0, _INVALID_COLLAPSED_RESPONSE_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_COLLAPSED_RESPONSE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_RX_LNK _0 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_lnk_fatal_1_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _RXHDROVFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXHDROVFERR, "RX HDR OVF Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_1, _RXHDROVFERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_HDR_OVERFLOW_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _RXDATAOVFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXDATAOVFERR, "RX Data OVF Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_1, _RXDATAOVFERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DATA_OVERFLOW_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _STOMPDETERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_STOMPDETERR, "Stomp Det Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLTLC, _RX_LNK_ERR_REPORT_INJECT_1, _STOMPDETERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_STOMP_DETECTED_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _RXPOISONERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXPOISONERR, "RX Poison Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_1,
                report.raw_enable ^ pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLTLC_RX_LNK _1 interrupts, link: %d pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, link, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

NvlStatus
_nvswitch_service_nvltlc_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            // An interrupt on an invalid link should never occur
            NVSWITCH_ASSERT(link != NULL);
            continue;
        }

        if (NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT) != nvlipt_instance)
        {
            NVSWITCH_ASSERT(0);
            break;
        }

        if (nvswitch_is_link_in_reset(device, link))
        {
            continue;
        }

        if (_nvswitch_service_nvltlc_tx_sys_fatal_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_sys_fatal_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_tx_lnk_fatal_0_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_fatal_0_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_fatal_1_lr10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

static NvlStatus
_nvswitch_service_nvlipt_common_fatal_lr10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NvU32 link, local_link_idx;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_NVLIPT_RD32_LR10(device, instance, _NVLIPT_COMMON, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_NVLIPT_RD32_LR10(device, instance, _NVLIPT_COMMON, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable &
        (DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _CLKCTL_ILLEGAL_REQUEST, 1) |
            DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _RSTSEQ_PLL_TIMEOUT, 1) |
            DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _RSTSEQ_PHYARB_TIMEOUT, 1));

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    error_event.nvliptInstance = (NvU8) instance;

    unhandled = pending;
    report.raw_first = NVSWITCH_NVLIPT_RD32_LR10(device, instance, _NVLIPT_COMMON, _ERR_FIRST_0);
    contain = NVSWITCH_NVLIPT_RD32_LR10(device, instance, _NVLIPT_COMMON, _ERR_CONTAIN_EN_0);
    injected = NVSWITCH_NVLIPT_RD32_LR10(device, instance, _NVLIPT_COMMON, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _CLKCTL_ILLEGAL_REQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT; local_link_idx++)
        {
            link = (instance * NVSWITCH_LINKS_PER_NVLIPT) + local_link_idx;
            if (nvswitch_is_link_valid(device, link))
            {
                NVSWITCH_REPORT_CONTAIN(_HW_NVLIPT_CLKCTL_ILLEGAL_REQUEST, "CLKCTL_ILLEGAL_REQUEST", NV_FALSE);
            }
        }

        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_COMMON, _ERR_REPORT_INJECT_0, _CLKCTL_ILLEGAL_REQUEST, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_CLKCTL_ILLEGAL_REQUEST_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _RSTSEQ_PLL_TIMEOUT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT; local_link_idx++)
        {
            link = (instance * NVSWITCH_LINKS_PER_NVLIPT) + local_link_idx;
            if (nvswitch_is_link_valid(device, link))
            {
                NVSWITCH_REPORT_CONTAIN(_HW_NVLIPT_RSTSEQ_PLL_TIMEOUT, "RSTSEQ_PLL_TIMEOUT", NV_FALSE);
            }
        }

        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_COMMON, _ERR_REPORT_INJECT_0, _RSTSEQ_PLL_TIMEOUT, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RSTSEQ_PLL_TIMEOUT_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _RSTSEQ_PHYARB_TIMEOUT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT; local_link_idx++)
        {
            link = (instance * NVSWITCH_LINKS_PER_NVLIPT) + local_link_idx;
            if (nvswitch_is_link_valid(device, link))
            {
                NVSWITCH_REPORT_CONTAIN(_HW_NVLIPT_RSTSEQ_PHYARB_TIMEOUT, "RSTSEQ_PHYARB_TIMEOUT", NV_FALSE);
            }
        }

        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_COMMON, _ERR_REPORT_INJECT_0, _RSTSEQ_PHYARB_TIMEOUT, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RSTSEQ_PHYARB_TIMEOUT_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT; local_link_idx++)
    {
        link = (instance * NVSWITCH_LINKS_PER_NVLIPT) + local_link_idx;
        if (nvswitch_is_link_valid(device, link) &&
            (device->link[link].fatal_error_occurred))
        {
            NVSWITCH_NVLIPT_WR32_LR10(device, instance, _NVLIPT_COMMON, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
            break;
        }
    }

    // clear the interrupts
    if (report.raw_first & report.mask)
    {
        NVSWITCH_NVLIPT_WR32_LR10(device, instance, _NVLIPT_COMMON, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }
    NVSWITCH_NVLIPT_WR32_LR10(device, instance, _NVLIPT_COMMON, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLIPT_COMMON FATAL interrupts, pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlipt_lnk_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(link);

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0);
    injected = NVSWITCH_LINK_RD32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_REPORT_INJECT_0);

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _SLEEPWHILEACTIVELINK, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLIPT_LNK_SLEEPWHILEACTIVELINK, "No non-empty link is detected", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _SLEEPWHILEACTIVELINK, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_SLEEP_WHILE_ACTIVE_LINK_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _RSTSEQ_PHYCTL_TIMEOUT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLIPT_LNK_RSTSEQ_PHYCTL_TIMEOUT, "Reset sequencer timed out waiting for a handshake from PHYCTL", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _RSTSEQ_PHYCTL_TIMEOUT, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RSTSEQ_PHYCTL_TIMEOUT_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _RSTSEQ_CLKCTL_TIMEOUT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLIPT_LNK_RSTSEQ_CLKCTL_TIMEOUT, "Reset sequencer timed out waiting for a handshake from CLKCTL", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _ERR_REPORT_INJECT_0, _RSTSEQ_CLKCTL_TIMEOUT, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RSTSEQ_CLKCTL_TIMEOUT_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable ^ pending);
    }

    // clear interrupts
    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LR10(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                "%s: Unhandled NVLIPT_LNK FATAL interrupts, pending: 0x%x enabled: 0x%x.\n",
                 __FUNCTION__, pending, report.raw_enable);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlipt_link_fatal_lr10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance
)
{
    NvU32 i, intrLink;
    NvU64 enabledLinkMask, localLinkMask, localEnabledLinkMask, interruptingLinks = 0;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(nvlipt_instance);
    localEnabledLinkMask = enabledLinkMask & localLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, localEnabledLinkMask)
    {
        intrLink = NVSWITCH_LINK_RD32_LR10(device, i, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);

        if(intrLink)
        {
            interruptingLinks |= NVBIT(i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if(interruptingLinks)
    {
        FOR_EACH_INDEX_IN_MASK(64, i, interruptingLinks)
        {
            if( _nvswitch_service_nvlipt_lnk_fatal_lr10(device, nvlipt_instance, i) != NVL_SUCCESS)
            {
                return -NVL_MORE_PROCESSING_REQUIRED;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
        return NVL_SUCCESS;
    }
    else
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }
}

static NvlStatus
_nvswitch_service_nvlipt_fatal_lr10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvlStatus status[6];

    //
    // MINION LINK interrupts trigger both INTR_FATAL and INTR_NONFATAL
    // trees (Bug 3037835). Because of this, we must service them in both the
    // fatal and nonfatal handlers
    //
    status[0] = device->hal.nvswitch_service_minion_link(device, instance);
    status[1] = _nvswitch_service_nvldl_fatal_lr10(device, instance);
    status[2] = _nvswitch_service_nvltlc_fatal_lr10(device, instance);
    status[3] = _nvswitch_service_minion_fatal_lr10(device, instance);
    status[4] = _nvswitch_service_nvlipt_common_fatal_lr10(device, instance);
    status[5] = _nvswitch_service_nvlipt_link_fatal_lr10(device, instance);

    if (status[0] != NVL_SUCCESS &&
        status[1] != NVL_SUCCESS &&
        status[2] != NVL_SUCCESS &&
        status[3] != NVL_SUCCESS &&
        status[4] != NVL_SUCCESS &&
        status[5] != NVL_SUCCESS)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_saw_fatal_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 pending, bit, unhandled;
    NvU32 i;

    pending = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_FATAL);
    pending &= chip_device->intr_enable_fatal;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    for (i = 0; i < NUM_NPG_ENGINE_LR10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, NPG, i))
        {
            continue;
        }

        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_FATAL, _NPG_0, 1) << i;
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_npg_fatal_lr10(device, i) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    for (i = 0; i < NUM_NXBAR_ENGINE_LR10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, NXBAR, i))
            continue;

        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_FATAL, _NXBAR_0, 1) << i;
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_nxbar_fatal_lr10(device, i) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    for (i = 0; i < NUM_NVLIPT_ENGINE_LR10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LR10(device, NVLIPT, i))
        {
            continue;
        }

        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_FATAL, _NVLIPT_0, 1) << i;
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_nvlipt_fatal_lr10(device, i) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    if (NVSWITCH_ENG_VALID_LR10(device, SOE, 0))
    {
        bit = DRF_NUM(_NVLSAW_NVSPMC, _INTR_FATAL, _SOE, 1);
        if (nvswitch_test_flags(pending, bit))
        {
            if (_nvswitch_service_soe_fatal_lr10(device) == NVL_SUCCESS)
            {
                nvswitch_clear_flags(&unhandled, bit);
            }
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_saw_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status[4];

    status[0] = _nvswitch_service_saw_legacy_lr10(device);
    status[1] = _nvswitch_service_saw_fatal_lr10(device);
    status[2] = _nvswitch_service_saw_nonfatal_lr10(device);

    if ((status[0] != NVL_SUCCESS) &&
        (status[1] != NVL_SUCCESS) &&
        (status[2] != NVL_SUCCESS))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_legacy_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 pending, bit, unhandled;

    pending = NVSWITCH_REG_RD32(device, _PSMC, _INTR_LEGACY);
    pending &= chip_device->intr_enable_legacy;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    bit = DRF_NUM(_PSMC, _INTR_LEGACY, _SAW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (_nvswitch_service_saw_lr10(device) == NVL_SUCCESS)
        {
            nvswitch_clear_flags(&unhandled, bit);
        }
    }

    bit = DRF_NUM(_PSMC, _INTR_LEGACY, _PRIV_RING, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (_nvswitch_service_priv_ring_lr10(device) == NVL_SUCCESS)
        {
            nvswitch_clear_flags(&unhandled, bit);
        }
    }

    bit = DRF_NUM(_PSMC, _INTR_LEGACY, _PBUS, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (_nvswitch_service_pbus_lr10(device) == NVL_SUCCESS)
        {
            nvswitch_clear_flags(&unhandled, bit);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

//
// Service interrupt and re-enable interrupts. Interrupts should disabled when
// this is called.
//
NvlStatus
nvswitch_lib_service_interrupts_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status;

    status = _nvswitch_service_legacy_lr10(device);

    /// @todo remove NVL_NOT_FOUND from the condition below, it was added as a WAR until Bug 2856055 is fixed.
    if ((status != NVL_SUCCESS) && (status != -NVL_NOT_FOUND))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    _nvswitch_rearm_msi_lr10(device);

    return NVL_SUCCESS;
}

