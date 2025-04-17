/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "regkey_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "cci/cci_nvswitch.h"

#include "ls10/ls10.h"
#include "ls10/minion_ls10.h"
#include "ls10/soe_ls10.h"

#include "nvswitch/ls10/dev_ctrl_ip.h"
#include "nvswitch/ls10/dev_pri_masterstation_ip.h"
#include "nvswitch/ls10/dev_pri_hub_sys_ip.h"
#include "nvswitch/ls10/dev_pri_hub_sysb_ip.h"
#include "nvswitch/ls10/dev_pri_hub_prt_ip.h"

#include "nvswitch/ls10/dev_npg_ip.h"
#include "nvswitch/ls10/dev_nport_ip.h"
#include "nvswitch/ls10/dev_route_ip.h"
#include "nvswitch/ls10/dev_ingress_ip.h"
#include "nvswitch/ls10/dev_sourcetrack_ip.h"
#include "nvswitch/ls10/dev_egress_ip.h"
#include "nvswitch/ls10/dev_tstate_ip.h"
#include "nvswitch/ls10/dev_multicasttstate_ip.h"
#include "nvswitch/ls10/dev_reductiontstate_ip.h"

#include "nvswitch/ls10/dev_nvlw_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_minion_ip_addendum.h"
#include "nvswitch/ls10/dev_cpr_ip.h"
#include "nvswitch/ls10/dev_nvlipt_ip.h"
#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_nvltlc_ip.h"
#include "nvswitch/ls10/dev_nvldl_ip.h"

#include "nvswitch/ls10/dev_nxbar_tcp_global_ip.h"
#include "nvswitch/ls10/dev_nxbar_tile_ip.h"
#include "nvswitch/ls10/dev_nxbar_tileout_ip.h"

#include "nvswitch/ls10/dev_ctrl_ip_addendum.h"

static void _nvswitch_create_deferred_link_errors_task_ls10(nvswitch_device *device, NvU32 nvlipt_instance, NvU32 link);

static void
_nvswitch_construct_ecc_error_event_ls10
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
    NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _MINION_INTR_NONSTALL_EN, 0);

     // Tree 0 (stall) is where we route _all_ MINION interrupts for now
    intrEn = DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FATAL,          _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _NONFATAL,       _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FALCON_STALL,   _ENABLE) |
             DRF_DEF(_MINION, _MINION_INTR_STALL_EN, _FALCON_NOSTALL, _DISABLE);

    for (i = 0; i < NVSWITCH_LINKS_PER_MINION_LS10; ++i)
    {
        // get the global link number of the link we are iterating over
        globalLink = (instance * NVSWITCH_LINKS_PER_MINION_LS10) + i;

        // the link is valid place bit in link mask
        if (device->link[globalLink].valid)
        {
            localDiscoveredLinks |= NVBIT(i);
        }
    }

    intrEn = FLD_SET_DRF_NUM(_MINION, _MINION_INTR_STALL_EN, _LINK,
                            localDiscoveredLinks, intrEn);

   {
        // Disable interrupts only if explicitly requested to.  Default to enable.
        if (device->regkeys.minion_intr != NV_SWITCH_REGKEY_MINION_INTERRUPTS_DISABLE)
        {
            NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _MINION_INTR_STALL_EN, intrEn);
        }
    }
}

static void
_nvswitch_initialize_nvlipt_interrupts_ls10
(
    nvswitch_device *device
)
{
    NvU32 i;
    NvU32 regval = 0;
    NvU64 link_enable_mask;

    //
    // NVLipt interrupt routing (NVLIPT_COMMON, NVLIPT_LNK, NVLDL, NVLTLC)
    // will be initialized by MINION NVLPROD flow
    //
    // We must enable interrupts at the top levels in NVLW, NVLIPT_COMMON,
    // NVLIPT_LNK and MINION
    //

    // NVLW
    regval = DRF_NUM(_NVLW_COMMON, _INTR_0_MASK, _FATAL,       0x1) |
             DRF_NUM(_NVLW_COMMON, _INTR_0_MASK, _NONFATAL,    0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_0_MASK, _CORRECTABLE, 0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_0_MASK, _INTR0,       0x1) |
             DRF_NUM(_NVLW_COMMON, _INTR_0_MASK, _INTR1,       0x0);
    NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_COMMON, _INTR_0_MASK, regval);

    regval = DRF_NUM(_NVLW_COMMON, _INTR_1_MASK, _FATAL,       0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_1_MASK, _NONFATAL,    0x1) |
             DRF_NUM(_NVLW_COMMON, _INTR_1_MASK, _CORRECTABLE, 0x1) |
             DRF_NUM(_NVLW_COMMON, _INTR_1_MASK, _INTR0,       0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_1_MASK, _INTR1,       0x1);
    NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_COMMON, _INTR_1_MASK, regval);

    regval = DRF_NUM(_NVLW_COMMON, _INTR_2_MASK, _FATAL,       0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_2_MASK, _NONFATAL,    0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_2_MASK, _CORRECTABLE, 0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_2_MASK, _INTR0,       0x0) |
             DRF_NUM(_NVLW_COMMON, _INTR_2_MASK, _INTR1,       0x0);
    NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_COMMON, _INTR_2_MASK, regval);

    // NVLW link
    for (i = 0; i < NV_NVLW_LINK_INTR_0_MASK__SIZE_1; i++)
    {
        regval = DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _FATAL,       0x1) |
                 DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _NONFATAL,    0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _CORRECTABLE, 0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR0,       0x1) |
                 DRF_NUM(_NVLW_LINK, _INTR_0_MASK, _INTR1,       0x0);
        NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_LINK, _INTR_0_MASK(i), regval);

        regval = DRF_NUM(_NVLW_LINK, _INTR_1_MASK, _FATAL,       0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_1_MASK, _NONFATAL,    0x1) |
                 DRF_NUM(_NVLW_LINK, _INTR_1_MASK, _CORRECTABLE, 0x1) |
                 DRF_NUM(_NVLW_LINK, _INTR_1_MASK, _INTR0,       0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_1_MASK, _INTR1,       0x1);
        NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_LINK, _INTR_1_MASK(i), regval);

        regval = DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _FATAL,       0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _NONFATAL,    0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _CORRECTABLE, 0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _INTR0,       0x0) |
                 DRF_NUM(_NVLW_LINK, _INTR_2_MASK, _INTR1,       0x0);
        NVSWITCH_BCAST_WR32_LS10(device, NVLW, _NVLW_LINK, _INTR_2_MASK(i), regval);
    }

    // NVLIPT_COMMON
    regval = DRF_NUM(_NVLIPT_COMMON, _INTR_CONTROL_COMMON, _INT0_EN, 0x1) |
             DRF_NUM(_NVLIPT_COMMON, _INTR_CONTROL_COMMON, _INT1_EN, 0x1);

    NVSWITCH_BCAST_WR32_LS10(device, NVLIPT, _NVLIPT_COMMON, _INTR_CONTROL_COMMON, regval);

    // NVLIPT_LNK
    regval = DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT0_EN, 0x1) |
             DRF_NUM(_NVLIPT_LNK, _INTR_CONTROL_LINK, _INT1_EN, 0x1);
    NVSWITCH_BCAST_WR32_LS10(device, NVLIPT_LNK, _NVLIPT_LNK, _INTR_CONTROL_LINK, regval);

    // NVLIPT_LNK_INTR_1
    regval = DRF_NUM(_NVLIPT_LNK, _INTR_INT1_EN, _LINKSTATEREQUESTREADYSET, 0x1);
    NVSWITCH_BCAST_WR32_LS10(device, NVLIPT_LNK, _NVLIPT_LNK, _INTR_INT1_EN, regval);

    // MINION
    for (i = 0; i < NUM_MINION_ENGINE_LS10; ++i)
    {
        if (!NVSWITCH_ENG_VALID_LS10(device, MINION, i))
        {
            continue;
        }

        _nvswitch_initialize_minion_interrupts(device,i);
    }

    // CPR

    regval = NVSWITCH_ENG_RD32(device, CPR, _BCAST, 0, _CPR_SYS, _ERR_LOG_EN_0);
    regval = FLD_SET_DRF(_CPR_SYS, _ERR_LOG_EN_0, _ENGINE_RESET_ERR, __PROD, regval);
    NVSWITCH_ENG_WR32(device, CPR, _BCAST, 0, _CPR_SYS, _ERR_LOG_EN_0, regval);

    regval = DRF_DEF(_CPR_SYS, _NVLW_INTR_0_MASK, _CPR_INTR, _ENABLE) |
          DRF_DEF(_CPR_SYS, _NVLW_INTR_0_MASK, _INTR0, _ENABLE);
    NVSWITCH_ENG_WR32(device, CPR, _BCAST, 0, _CPR_SYS, _NVLW_INTR_0_MASK, regval);

    regval = DRF_DEF(_CPR_SYS, _NVLW_INTR_1_MASK, _CPR_INTR, _DISABLE) |
          DRF_DEF(_CPR_SYS, _NVLW_INTR_1_MASK, _INTR1, _ENABLE);
    NVSWITCH_ENG_WR32(device, CPR, _BCAST, 0, _CPR_SYS, _NVLW_INTR_1_MASK, regval);

    regval = DRF_DEF(_CPR_SYS, _NVLW_INTR_2_MASK, _CPR_INTR, _DISABLE) |
          DRF_DEF(_CPR_SYS, _NVLW_INTR_2_MASK, _INTR2, _ENABLE);
    NVSWITCH_ENG_WR32(device, CPR, _BCAST, 0, _CPR_SYS, _NVLW_INTR_2_MASK, regval);

    //
    // Disable engine interrupts requested by regkey "LinkEnableMask".
    // All the links are enabled by default.
    //
    link_enable_mask = ((NvU64)device->regkeys.link_enable_mask2 << 32 |
        (NvU64)device->regkeys.link_enable_mask);

    for (i = 0; i < NVSWITCH_NUM_LINKS_LS10; i++)
    {
        if ((NVBIT64(i) & link_enable_mask) == 0)
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: Disabling interrupts for link #%d\n",
                __FUNCTION__, i);
            nvswitch_link_disable_interrupts_ls10(device, i);
        }
    }
}

static void
_nvswitch_initialize_route_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.route.fatal =
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _ROUTEBUFERR, _ENABLE)          |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _GLT_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _PDCTRLPARERR, _ENABLE)         |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _NVS_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _CDTPARERR, _ENABLE)            |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _MCRID_ECC_DBE_ERR, _ENABLE)    |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _EXTMCRID_ECC_DBE_ERR, _ENABLE) |
        DRF_DEF(_ROUTE, _ERR_FATAL_REPORT_EN_0, _RAM_ECC_DBE_ERR, _ENABLE);

    chip_device->intr_mask.route.nonfatal =
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _NOPORTDEFINEDERR, _ENABLE)         |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _INVALIDROUTEPOLICYERR, _ENABLE)    |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _GLT_ECC_LIMIT_ERR, _ENABLE)        |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _NVS_ECC_LIMIT_ERR, _ENABLE)        |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _MCRID_ECC_LIMIT_ERR, _ENABLE)      |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _EXTMCRID_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _RAM_ECC_LIMIT_ERR, _ENABLE)        |
        DRF_DEF(_ROUTE, _ERR_NON_FATAL_REPORT_EN_0, _INVALID_MCRID_ERR, _ENABLE);
    // NOTE: _MC_TRIGGER_ERR is debug-use only
}

static void
_nvswitch_initialize_ingress_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.ingress[0].fatal =
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _CMDDECODEERR, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ECC_DBE_ERR, _ENABLE)  |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_DBE_ERR, _ENABLE)    |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _INVALIDVCSET, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _REMAPTAB_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RIDTAB_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _RLANTAB_ECC_DBE_ERR, _ENABLE)       |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_PARITY_ERR, _ENABLE)         |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ECC_DBE_ERR, _ENABLE)  |
        DRF_DEF(_INGRESS, _ERR_FATAL_REPORT_EN_0, _MCREMAPTAB_ECC_DBE_ERR, _ENABLE);

    chip_device->intr_mask.ingress[0].nonfatal =
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REQCONTEXTMISMATCHERR, _ENABLE)    |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ACLFAIL, _ENABLE)                  |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NCISOC_HDR_ECC_LIMIT_ERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRBOUNDSERR, _ENABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RIDTABCFGERR, _ENABLE)             |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RLANTABCFGERR, _ENABLE)            |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _REMAPTAB_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RIDTAB_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RLANTAB_ECC_LIMIT_ERR, _ENABLE)    |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _ADDRTYPEERR, _ENABLE)              |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_INDEX_ERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_INDEX_ERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_INDEX_ERR, _ENABLE)     |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_REQCONTEXTMISMATCHERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_REQCONTEXTMISMATCHERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_REQCONTEXTMISMATCHERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ACLFAIL, _ENABLE)     |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ACLFAIL, _ENABLE)     |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_ACLFAIL, _ENABLE)       |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTAREMAPTAB_ADDRBOUNDSERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _EXTBREMAPTAB_ADDRBOUNDSERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_0, _MCREMAPTAB_ADDRBOUNDSERR, _ENABLE);

    chip_device->intr_mask.ingress[1].fatal = 0;

    chip_device->intr_mask.ingress[1].nonfatal =
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTAREMAPTAB_ECC_LIMIT_ERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTBREMAPTAB_ECC_LIMIT_ERR, _ENABLE) |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREMAPTAB_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCCMDTOUCADDRERR, _ENABLE)           |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _READMCREFLECTMEMERR, _ENABLE)        |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTAREMAPTAB_ADDRTYPEERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _EXTBREMAPTAB_ADDRTYPEERR, _ENABLE)   |
        DRF_DEF(_INGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREMAPTAB_ADDRTYPEERR, _ENABLE);
}

static void
_nvswitch_initialize_egress_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.egress[0].fatal =
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _EGRESSBUFERR, _ENABLE)                 |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _PKTROUTEERR, _ENABLE)                  |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _SEQIDERR, _ENABLE)                     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_DBE_ERR, _ENABLE)      |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOCCREDITOVFL, _ENABLE)             |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _REQTGTIDMISMATCHERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _RSPREQIDMISMATCHERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_HDR_PARITY_ERR, _ENABLE)         |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NCISOC_CREDIT_PARITY_ERR, _ENABLE)     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_FLITTYPE_MISMATCH_ERR, _ENABLE)  |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _CREDIT_TIME_OUT_ERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _INVALIDVCSET_ERR, _ENABLE)             |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _NXBAR_SIDEBAND_PD_PARITY_ERR, _ENABLE) |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _URRSPERR, _ENABLE)                     |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_0, _HWRSPERR, _ENABLE);

    chip_device->intr_mask.egress[0].nonfatal =
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _NXBAR_HDR_ECC_LIMIT_ERR, _ENABLE)     |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _PRIVRSPERR, _ENABLE)                  |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_0, _RFU, _DISABLE);

    chip_device->intr_mask.egress[1].fatal =

        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_1, _MCRSPCTRLSTORE_ECC_DBE_ERR, _ENABLE)              |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_1, _RBCTRLSTORE_ECC_DBE_ERR, _ENABLE)                 |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_1, _MCREDSGT_ECC_DBE_ERR, _ENABLE)                    |
        DRF_DEF(_EGRESS, _ERR_FATAL_REPORT_EN_1, _MCRSP_RAM_HDR_ECC_DBE_ERR, _ENABLE);

    chip_device->intr_mask.egress[1].nonfatal =
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, _ENABLE)       |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCRSPCTRLSTORE_ECC_LIMIT_ERR, _ENABLE)            |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _RBCTRLSTORE_ECC_LIMIT_ERR, _ENABLE)               |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREDSGT_ECC_LIMIT_ERR, _ENABLE)                  |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREDBUF_ECC_LIMIT_ERR, _ENABLE)                  |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCRSP_RAM_HDR_ECC_LIMIT_ERR, _ENABLE)             |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _NXBAR_REDUCTION_HDR_ECC_DBE_ERR, _ENABLE)         |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _NXBAR_REDUCTION_HDR_PARITY_ERR, _ENABLE)          |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _NXBAR_REDUCTION_FLITTYPE_MISMATCH_ERR, _ENABLE)   |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCREDBUF_ECC_DBE_ERR, _ENABLE)                    |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _MCRSP_CNT_ERR, _ENABLE)                           |
        DRF_DEF(_EGRESS, _ERR_NON_FATAL_REPORT_EN_1, _RBRSP_CNT_ERR, _ENABLE);
}

static void
_nvswitch_initialize_tstate_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.tstate.fatal =
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOLBUFERR, _ENABLE)              |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOL_ECC_DBE_ERR, _ENABLE)        |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTOREBUFERR, _ENABLE)           |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_DBE_ERR, _ENABLE)     |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _ATO_ERR, _ENABLE)                    |
        DRF_DEF(_TSTATE, _ERR_FATAL_REPORT_EN_0, _CAMRSP_ERR, _ENABLE);

    chip_device->intr_mask.tstate.nonfatal =
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOL_ECC_LIMIT_ERR, _ENABLE)      |
        DRF_DEF(_TSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE);
}

static void
_nvswitch_initialize_sourcetrack_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.sourcetrack.fatal =
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, _ENABLE) |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _DUP_CREQ_TCEN0_TAG_ERR, _ENABLE)     |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _INVALID_TCEN0_RSP_ERR, _ENABLE)      |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _INVALID_TCEN1_RSP_ERR, _ENABLE)      |
        DRF_DEF(_SOURCETRACK, _ERR_FATAL_REPORT_EN_0, _SOURCETRACK_TIME_OUT_ERR, _ENABLE);

    chip_device->intr_mask.sourcetrack.nonfatal =
        DRF_DEF(_SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE);
}

static void
_nvswitch_initialize_multicast_tstate_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.mc_tstate.fatal =
        DRF_DEF(_MULTICASTTSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOL_ECC_DBE_ERR, _ENABLE)             |
        DRF_DEF(_MULTICASTTSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_BUF_OVERWRITE_ERR, _ENABLE)    |
        DRF_DEF(_MULTICASTTSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_DBE_ERR, _ENABLE);

    chip_device->intr_mask.mc_tstate.nonfatal =
        DRF_DEF(_MULTICASTTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOL_ECC_LIMIT_ERR, _ENABLE)       |
        DRF_DEF(_MULTICASTTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE)    |
        DRF_DEF(_MULTICASTTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_MCTO_ERR, _ENABLE);
}

static void
_nvswitch_initialize_reduction_tstate_interrupts
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    chip_device->intr_mask.red_tstate.fatal =
        DRF_DEF(_REDUCTIONTSTATE, _ERR_FATAL_REPORT_EN_0, _TAGPOOL_ECC_DBE_ERR, _ENABLE)            |
        DRF_DEF(_REDUCTIONTSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_BUF_OVERWRITE_ERR, _ENABLE)   |
        DRF_DEF(_REDUCTIONTSTATE, _ERR_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_DBE_ERR, _ENABLE);

    chip_device->intr_mask.red_tstate.nonfatal =
        DRF_DEF(_REDUCTIONTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _TAGPOOL_ECC_LIMIT_ERR, _ENABLE)      |
        DRF_DEF(_REDUCTIONTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_ECC_LIMIT_ERR, _ENABLE)   |
        DRF_DEF(_REDUCTIONTSTATE, _ERR_NON_FATAL_REPORT_EN_0, _CRUMBSTORE_RTO_ERR, _ENABLE);
}

void
_nvswitch_initialize_nport_interrupts_ls10
(
    nvswitch_device *device
)
{
// Moving this L2 register access to SOE. Refer bug #3747687
#if 0
    NvU32 val;

    val =
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE, 1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE, 1);
    NVSWITCH_NPORT_BCAST_WR32_LS10(device, _NPORT, _ERR_CONTROL_COMMON_NPORT, val);
#endif // 0

    _nvswitch_initialize_route_interrupts(device);
    _nvswitch_initialize_ingress_interrupts(device);
    _nvswitch_initialize_egress_interrupts(device);
    _nvswitch_initialize_tstate_interrupts(device);
    _nvswitch_initialize_sourcetrack_interrupts(device);
    _nvswitch_initialize_multicast_tstate_interrupts(device);
    _nvswitch_initialize_reduction_tstate_interrupts(device);
}

void
_nvswitch_initialize_nxbar_interrupts_ls10
(
    nvswitch_device *device
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 report_fatal;

    report_fatal =
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_BUFFER_OVERFLOW, 1)     |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_BUFFER_UNDERFLOW, 1)    |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _EGRESS_CREDIT_OVERFLOW, 1)      |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _EGRESS_CREDIT_UNDERFLOW, 1)     |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_NON_BURSTY_PKT, 1)      |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_NON_STICKY_PKT, 1)      |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_BURST_GT_9_DATA_VC, 1)  |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_PKT_INVALID_DST, 1)     |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_PKT_PARITY_ERROR, 1)    |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_SIDEBAND_PARITY_ERROR, 1) |
        DRF_NUM(_NXBAR_TILE, _ERR_FATAL_INTR_EN, _INGRESS_REDUCTION_PKT_ERROR, 1);

// Moving this L2 register access to SOE. Refer bug #3747687
#if 0
    NVSWITCH_BCAST_WR32_LS10(device, NXBAR, _NXBAR_TILE, _ERR_FATAL_INTR_EN, report_fatal);
#endif // 0

    chip_device->intr_mask.tile.fatal = report_fatal;
    chip_device->intr_mask.tile.nonfatal = 0;

    report_fatal =
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _INGRESS_BUFFER_OVERFLOW, 1)     |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _INGRESS_BUFFER_UNDERFLOW, 1)    |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _EGRESS_CREDIT_OVERFLOW, 1)      |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _EGRESS_CREDIT_UNDERFLOW, 1)     |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _INGRESS_NON_BURSTY_PKT, 1)      |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _INGRESS_NON_STICKY_PKT, 1)      |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _INGRESS_BURST_GT_9_DATA_VC, 1)  |
        DRF_NUM(_NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, _EGRESS_CDT_PARITY_ERROR, 1);

// Moving this L2 register access to SOE. Refer bug #3747687
#if 0
    NVSWITCH_BCAST_WR32_LS10(device, NXBAR, _NXBAR_TILEOUT, _ERR_FATAL_INTR_EN, report_fatal);
#endif // 0

    chip_device->intr_mask.tileout.fatal = report_fatal;
    chip_device->intr_mask.tileout.nonfatal = 0;
}

/*
 * @brief Service MINION Falcon interrupts on the requested interrupt tree
 *        Falcon Interrupts are a little unique in how they are handled:#include <assert.h>
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
nvswitch_minion_service_falcon_interrupts_ls10
(
    nvswitch_device *device,
    NvU32           instance
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled, intr, link;

    link = instance * NVSWITCH_LINKS_PER_MINION_LS10;
    report.raw_pending = NVSWITCH_MINION_RD32_LS10(device, instance, _CMINION, _FALCON_IRQSTAT);
    report.raw_enable = chip_device->intr_minion_dest;
    report.mask = NVSWITCH_MINION_RD32_LS10(device, instance, _CMINION, _FALCON_IRQMASK);

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
        intr = NVSWITCH_MINION_RD32_LS10(device, instance, _MINION, _MINION_INTR_STALL_EN);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _FATAL, _DISABLE, intr);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _FALCON_STALL, _DISABLE, intr);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _FATAL, _DISABLE, intr);
        intr = FLD_SET_DRF(_MINION, _MINION_INTR_STALL_EN, _NONFATAL, _DISABLE, intr);
        NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _MINION_INTR_STALL_EN, intr);
    }

    // Write to IRQSCLR to clear status of interrupt
    NVSWITCH_MINION_WR32_LS10(device, instance, _CMINION, _FALCON_IRQSCLR, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Send priv ring command and wait for completion
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 * @param[in] cmd           encoded priv ring command
 */
static NvlStatus
_nvswitch_ring_master_cmd_ls10
(
    nvswitch_device *device,
    NvU32 cmd
)
{
    NvU32 value;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    NVSWITCH_ENG_WR32(device, PRI_MASTER_RS, , 0, _PPRIV_MASTER, _RING_COMMAND, cmd);

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        value = NVSWITCH_ENG_RD32(device, PRI_MASTER_RS, , 0, _PPRIV_MASTER, _RING_COMMAND);
        if (FLD_TEST_DRF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _NO_CMD, value))
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (!FLD_TEST_DRF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _NO_CMD, value))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout waiting for RING_COMMAND == NO_CMD (cmd=0x%x).\n",
            __FUNCTION__, cmd);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_priv_ring_ls10
(
    nvswitch_device *device
)
{
    NvU32 pending, i;
    NVSWITCH_PRI_ERROR_LOG_TYPE pri_error;
    NvlStatus status = NVL_SUCCESS;

    pending = NVSWITCH_ENG_RD32(device, PRI_MASTER_RS, , 0, _PPRIV_MASTER, _RING_INTERRUPT_STATUS0);
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    //
    // SYS
    //

    if (FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYS, 1, pending))
    {
        pri_error.addr = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_ADR);
        pri_error.data = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_WRDAT);
        pri_error.info = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_INFO);
        pri_error.code = NVSWITCH_ENG_RD32(device, SYS_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_CODE);

        NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_ERROR, "PRI WRITE SYS error", NVSWITCH_PPRIV_WRITE_SYS, 0, pri_error);

        NVSWITCH_PRINT(device, ERROR,
            "SYS PRI write error addr: 0x%08x data: 0x%08x info: 0x%08x code: 0x%08x\n",
            pri_error.addr, pri_error.data,
            pri_error.info, pri_error.code);

        pending = FLD_SET_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYS, 0, pending);
    }

    //
    // SYSB
    //

    if (FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYSB, 1, pending))
    {
        pri_error.addr = NVSWITCH_ENG_RD32(device, SYSB_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_ADR);
        pri_error.data = NVSWITCH_ENG_RD32(device, SYSB_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_WRDAT);
        pri_error.info = NVSWITCH_ENG_RD32(device, SYSB_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_INFO);
        pri_error.code = NVSWITCH_ENG_RD32(device, SYSB_PRI_HUB, , 0, _PPRIV_SYS, _PRIV_ERROR_CODE);

        NVSWITCH_REPORT_PRI_ERROR_NONFATAL(_HW_HOST_PRIV_ERROR, "PRI WRITE SYSB error", NVSWITCH_PPRIV_WRITE_SYS, 1, pri_error);

        NVSWITCH_PRINT(device, ERROR,
            "SYSB PRI write error addr: 0x%08x data: 0x%08x info: 0x%08x code: 0x%08x\n",
            pri_error.addr, pri_error.data,
            pri_error.info, pri_error.code);

        pending = FLD_SET_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_SYSB, 0, pending);
    }

    //
    // per-PRT
    //

    for (i = 0; i < NUM_PRT_PRI_HUB_ENGINE_LS10; i++)
    {
        if (DRF_VAL(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
            _GBL_WRITE_ERROR_FBP, pending) & NVBIT(i))
        {
            pri_error.addr = NVSWITCH_ENG_RD32(device, PRT_PRI_HUB, , i, _PPRIV_PRT, _PRIV_ERROR_ADR);
            pri_error.data = NVSWITCH_ENG_RD32(device, PRT_PRI_HUB, , i, _PPRIV_PRT, _PRIV_ERROR_WRDAT);
            pri_error.info = NVSWITCH_ENG_RD32(device, PRT_PRI_HUB, , i, _PPRIV_PRT, _PRIV_ERROR_INFO);
            pri_error.code = NVSWITCH_ENG_RD32(device, PRT_PRI_HUB, , i, _PPRIV_PRT, _PRIV_ERROR_CODE);

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

    // acknowledge the interrupt to the ringmaster
    status = _nvswitch_ring_master_cmd_ls10(device,
        DRF_DEF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _ACK_INTERRUPT));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Timeout ACK'ing PRI error\n");
        //
        // Don't return error code -- there is nothing kernel SW can do about it if ACK failed.
        // Likely it is PLM protected and SOE needs to handle it.
        //
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_collect_nport_error_info_ls10
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
_nvswitch_collect_error_info_ls10
(
    nvswitch_device    *device,
    NvU32               link,
    NvU32               collect_flags,  // NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_*
    NVSWITCH_RAW_ERROR_LOG_TYPE *data
)
{
    NvU32 val;
    NvU32 i = 0;
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
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_ROUTE_ERR_TIMESTAMP_LOG,
                     NV_ROUTE_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME;
            NVSWITCH_PRINT(device, INFO,
                "ROUTE: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
    }

    val = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_HEADER_LOG_VALID);
    if (FLD_TEST_DRF_NUM(_ROUTE, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_ROUTE_ERR_MISC_LOG_0,
                         NV_ROUTE_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "ROUTE: MISC: 0x%08x\n", data->data[i-1]);
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_ROUTE_ERR_HEADER_LOG_4,
                         NV_ROUTE_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "ROUTE: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
                    data->data[i-7], data->data[i-6], data->data[i-5], data->data[i-4],
                    data->data[i-3], data->data[i-2], data->data[i-1]);
            }
        }
    }

    // INGRESS
    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME)
    {
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_INGRESS_ERR_TIMESTAMP_LOG,
                     NV_INGRESS_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME;
            NVSWITCH_PRINT(device, INFO,
                "INGRESS: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
    }

    val = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_HEADER_LOG_VALID);
    if (FLD_TEST_DRF_NUM(_INGRESS, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_INGRESS_ERR_MISC_LOG_0,
                         NV_INGRESS_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "INGRESS: MISC: 0x%08x\n", data->data[i-1]);
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_INGRESS_ERR_HEADER_LOG_4,
                         NV_INGRESS_ERR_HEADER_LOG_9);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "INGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x,\n",
                    data->data[i-6], data->data[i-5], data->data[i-4], data->data[i-3],
                    data->data[i-2], data->data[i-1]);
            }
        }
    }

    // EGRESS
    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME)
    {
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_EGRESS_ERR_TIMESTAMP_LOG,
                     NV_EGRESS_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME;
            NVSWITCH_PRINT(device, INFO,
                "EGRESS: TIMESTAMP: 0x%08x\n", data->data[i-1]);
        }
    }

    val = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_HEADER_LOG_VALID);
    if (FLD_TEST_DRF_NUM(_EGRESS, _ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_EGRESS_ERR_MISC_LOG_0,
                         NV_EGRESS_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: MISC: 0x%08x\n", data->data[i-1]);
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_EGRESS_ERR_HEADER_LOG_4,
                         NV_EGRESS_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
                    data->data[i-7], data->data[i-6], data->data[i-5], data->data[i-4],
                    data->data[i-3], data->data[i-2], data->data[i-1]);
            }
        }
    }

    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_TIME)
    {
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_EGRESS_MC_ERR_TIMESTAMP_LOG,
                     NV_EGRESS_MC_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_TIME;
            NVSWITCH_PRINT(device, INFO,
                "EGRESS: TIME MC: 0x%08x\n", data->data[i-1]);
        }
    }

    val = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _MC_ERR_HEADER_LOG_VALID);
    if (FLD_TEST_DRF_NUM(_EGRESS, _MC_ERR_HEADER_LOG_VALID, _HEADERVALID0, 1, val))
    {
        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_MISC)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_EGRESS_MC_ERR_MISC_LOG_0,
                         NV_EGRESS_MC_ERR_MISC_LOG_0);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_MISC;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS: MISC MC: 0x%08x\n", data->data[i-1]);
            }
        }

        if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_HDR)
        {
            status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                         NV_EGRESS_MC_ERR_HEADER_LOG_4,
                         NV_EGRESS_MC_ERR_HEADER_LOG_10);
            if (status == NVL_SUCCESS)
            {
                data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_HDR;
                NVSWITCH_PRINT(device, INFO,
                    "EGRESS MC: HEADER: 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
                    data->data[i-7], data->data[i-6], data->data[i-5], data->data[i-4],
                    data->data[i-3], data->data[i-2], data->data[i-1]);
            }
        }
    }

    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_MC_TIME)
    {
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_MULTICASTTSTATE_ERR_TIMESTAMP_LOG,
                     NV_MULTICASTTSTATE_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_MC_TIME;
            NVSWITCH_PRINT(device, INFO,
                "MC TSTATE MC: 0x%08x\n",
                data->data[i-1]);
        }
    }

    if (collect_flags & NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_RED_TIME)
    {
        status = _nvswitch_collect_nport_error_info_ls10(device, link, data, &i,
                     NV_REDUCTIONTSTATE_ERR_TIMESTAMP_LOG,
                     NV_REDUCTIONTSTATE_ERR_TIMESTAMP_LOG);
        if (status == NVL_SUCCESS)
        {
            data->flags |= NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_RED_TIME;
            NVSWITCH_PRINT(device, INFO,
                "MC TSTATE RED: 0x%08x\n",
                data->data[i-1]);
        }
    }

    while (i < NVSWITCH_RAW_ERROR_LOG_DATA_SIZE)
    {
        data->data[i++] = 0;
    }
}

static NvlStatus
_nvswitch_service_route_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.route.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _ROUTEBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_ROUTEBUFERR, "route buffer over/underflow", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_ROUTEBUFERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _GLT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE,
                _ERR_GLT_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_ROUTE_ERR_GLT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE,
                                               _ERR_GLT_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, "route GLT DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_GLT_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _PDCTRLPARERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_PDCTRLPARERR, "route parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_PDCTRLPARERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, "route incoming DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_LIMIT_ERR, 1))
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_STATUS_0,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _NVS_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _CDTPARERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_CDTPARERR, "route credit parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_CDTPARERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_CDTPARERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _MCRID_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_MCRID_ECC_DBE_ERR, "MC route ECC", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_MCRID_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_MCRID_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _EXTMCRID_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_EXTMCRID_ECC_DBE_ERR, "Extd MC route ECC", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_EXTMCRID_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_EXTMCRID_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_ROUTE_RAM_ECC_DBE_ERR, "route RAM ECC", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_ROUTE_RAM_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_ROUTE_RAM_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_ROUTE_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_route_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.route.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_FIRST_0);

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _NOPORTDEFINEDERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NOPORTDEFINEDERR, "route undefined route");
        NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_NOPORTDEFINEDERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _INVALIDROUTEPOLICYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_HDR,
            &data);
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
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_ROUTE_ERR_NVS_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
                &data);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, "route incoming ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _GLT_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _GLT_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_GLT_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_ROUTE_ERR_GLT_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //            	
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
                &data);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, "GLT ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_GLT_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_GLT_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _MCRID_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _MCRID_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_MCRID_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_ROUTE_ERR_MCRID_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
                &data);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, "MCRID ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_MCRID_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_MCRID_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _EXTMCRID_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _EXTMCRID_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_EXTMCRID_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_ROUTE_ERR_EXTMCRID_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
                &data);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_NVS_ECC_LIMIT_ERR, "EXTMCRID ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_EXTMCRID_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_EXTMCRID_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_ROUTE, _ERR_STATUS_0, _RAM_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _ROUTE, _ERR_RAM_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_ROUTE_ERR_RAM_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
                &data);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_RAM_ECC_LIMIT_ERR, "RAM ECC limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_RAM_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_ROUTE_RAM_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_ROUTE, _ERR_STATUS_0, _INVALID_MCRID_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_ROUTE_TIME,
            &data);
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_ROUTE_INVALID_MCRID_ERR, "invalid MC route");
        NVSWITCH_REPORT_DATA(_HW_NPORT_ROUTE_INVALID_MCRID_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_ROUTE_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );

        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_STATUS_0, pending);

    //
    // Note, when traffic is flowing, if we reset ERR_COUNT before ERR_STATUS
    // register, we won't see an interrupt again until counter wraps around.
    // In that case, we will miss writing back many ECC victim entries. Hence,
    // always clear _ERR_COUNT only after _ERR_STATUS register is cleared!
    //
    NVSWITCH_ENG_WR32(device, NPORT, , link, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER, 0x0);

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
_nvswitch_service_ingress_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.ingress[0].fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_ls10(device, link,
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

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTAREMAPTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTAREMAPTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTAREMAPTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_DBE_ERR, "ingress ExtA remap DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, "ingress header DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_INGRESS, _ERR_STATUS_0, _NCISOC_HDR_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_NCISOC_HDR_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_STATUS_0,
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
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                _ERR_REMAPTAB_ECC_ERROR_ADDRESS);

        if (FLD_TEST_DRF(_INGRESS_ERR_REMAPTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                                               _ERR_REMAPTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, "ingress Remap DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_REMAPTAB_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RIDTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                _ERR_RIDTAB_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_INGRESS_ERR_RIDTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                                               _ERR_RIDTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, "ingress RID DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTAB_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RLANTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                _ERR_RLANTAB_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_INGRESS_ERR_RLANTAB, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS,
                                               _ERR_RLANTAB_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR, "ingress RLAN DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_RLANTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
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

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_NCISOC_PARITY_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTBREMAPTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTBREMAPTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTBREMAPTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_DBE_ERR, "ingress ExtB remap DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_MCREMAPTAB_ECC_ERROR_COUNTER);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_MCREMAPTAB_ECC_ERROR_ADDRESS);
        report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_MCREMAPTAB_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_INGRESS_MCREMAPTAB_ECC_DBE_ERR, "ingress MC remap DBE", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_INGRESS_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_ingress_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NvU32 pending_0, pending_1;
    NvU32 raw_pending_0;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status = NVL_SUCCESS;

    //
    // _ERR_STATUS_0
    //
    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.ingress[0].nonfatal;

    raw_pending_0 = report.raw_pending;
    pending = (report.raw_pending & report.mask);
    pending_0 = pending;

    if (pending == 0)
    {
        goto _nvswitch_service_ingress_nonfatal_ls10_err_status_1;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_FIRST_0);
    _nvswitch_collect_error_info_ls10(device, link,
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
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_NCISOC_HDR_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR, "ingress header ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_NCISOC_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
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


    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _REMAPTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _REMAPTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_REMAPTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_REMAPTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_REMAPTAB_ECC_LIMIT_ERR, "ingress remap ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_REMAPTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_REMAPTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RIDTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _RIDTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RIDTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_RIDTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_RIDTAB_ECC_LIMIT_ERR, "ingress RID ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_RIDTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_RIDTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _RLANTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _RLANTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_RLANTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_RLANTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_RLANTAB_ECC_LIMIT_ERR, "ingress RLAN ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_RLANTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_RLANTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }


    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _ADDRTYPEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_ADDRTYPEERR, "ingress illegal address");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_ADDRTYPEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }


    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_INDEX_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_INDEX_ERR, "ingress ExtA remap index");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_INDEX_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_INDEX_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_INDEX_ERR, "ingress ExtB remap index");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_INDEX_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_INDEX_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_INDEX_ERR, "ingress MC remap index");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_INDEX_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_REQCONTEXTMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_REQCONTEXTMISMATCHERR, "ingress ExtA request context mismatch");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_REQCONTEXTMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_REQCONTEXTMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_REQCONTEXTMISMATCHERR, "ingress ExtB request context mismatch");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_REQCONTEXTMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_REQCONTEXTMISMATCHERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_REQCONTEXTMISMATCHERR, "ingress MC request context mismatch");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_REQCONTEXTMISMATCHERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_ACLFAIL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_ACLFAIL, "ingress invalid ExtA ACL");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_ACLFAIL, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_ACLFAIL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_ACLFAIL, "ingress invalid ExtB ACL");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_ACLFAIL, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_ACLFAIL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_ACLFAIL, "ingress invalid MC ACL");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_ACLFAIL, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_ADDRBOUNDSERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRBOUNDSERR, "ingress ExtA address bounds");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRBOUNDSERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_ADDRBOUNDSERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRBOUNDSERR, "ingress ExtB address bounds");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRBOUNDSERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_ADDRBOUNDSERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_ADDRBOUNDSERR, "ingress MC address bounds");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_ADDRBOUNDSERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_INGRESS_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

_nvswitch_service_ingress_nonfatal_ls10_err_status_1:
    //
    // _ERR_STATUS_1
    //
    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable & chip_device->intr_mask.ingress[1].nonfatal;

    pending = (report.raw_pending & report.mask);
    pending_1 = pending;

    if ((pending_0 == 0) && (pending_1 == 0))
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_FIRST_1);

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _EXTAREMAPTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(raw_pending_0,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTAREMAPTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTAREMAPTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_EXTAREMAPTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_LIMIT_ERR, "ingress ExtA remap ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_EXTAREMAPTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _EXTBREMAPTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(raw_pending_0,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _EXTBREMAPTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_EXTBREMAPTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_EXTBREMAPTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_LIMIT_ERR, "ingress ExtB remap ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_EXTBREMAPTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _MCREMAPTAB_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(raw_pending_0,
                DRF_NUM(_INGRESS, _ERR_STATUS_0, _MCREMAPTAB_ECC_DBE_ERR, 1))))
        {
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _INGRESS, _ERR_MCREMAPTAB_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_INGRESS_ERR_MCREMAPTAB_ECC_ERROR_COUNTER, 
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_ECC_LIMIT_ERR, "ingress MC remap ECC");
            NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_INGRESS_MCREMAPTAB_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _MCCMDTOUCADDRERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCCMDTOUCADDRERR, "ingress MC command to uc");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCCMDTOUCADDRERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _READMCREFLECTMEMERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_READMCREFLECTMEMERR, "ingress read reflective");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_READMCREFLECTMEMERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _EXTAREMAPTAB_ADDRTYPEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRTYPEERR, "ingress ExtA address type");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTAREMAPTAB_ADDRTYPEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _EXTBREMAPTAB_ADDRTYPEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRTYPEERR, "ingress ExtB address type");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_EXTBREMAPTAB_ADDRTYPEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_INGRESS, _ERR_STATUS_1, _MCREMAPTAB_ADDRTYPEERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_INGRESS_MCREMAPTAB_ADDRTYPEERR, "ingress MC address type");
        NVSWITCH_REPORT_DATA(_HW_NPORT_INGRESS_MCREMAPTAB_ADDRTYPEERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_INGRESS_ERR_NON_FATAL_REPORT_EN_1,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_FIRST_1,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_STATUS_0, pending_0);
    NVSWITCH_ENG_WR32(device, NPORT, , link, _INGRESS, _ERR_STATUS_1, pending_1);

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

    return status;
}

//
// Tstate
//

static NvlStatus
_nvswitch_service_tstate_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.tstate.nonfatal;
    report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_MISC_LOG_0);
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_FIRST_0);

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                    _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_TSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                                                   _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR, "TS tag store single-bit threshold");
            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
                &data);
            NVSWITCH_REPORT_DATA(_HW_NPORT_TSTATE_TAGPOOL_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_TSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
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
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                    _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_TSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                                                   _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR, "TS crumbstore single-bit threshold");
            _nvswitch_collect_error_info_ls10(device, link,
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
                NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
                &data);
            NVSWITCH_REPORT_DATA(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_TSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
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
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_TSTATE_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_tstate_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.tstate.fatal;
    report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_MISC_LOG_0);
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOLBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_TAGPOOLBUFERR, "TS pointer crossover", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
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
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_TSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                                               _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
            DRF_DEF(_TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, "TS tag store fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_TSTATE_TAGPOOL_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_TSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTOREBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CRUMBSTOREBUFERR, "TS crumbstore", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
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
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_TSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE,
                                               _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
            DRF_DEF(_TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, "TS crumbstore fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_TSTATE_CRUMBSTORE_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_TSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0,
                DRF_NUM(_TSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _ATO_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        if (FLD_TEST_DRF_NUM(_TSTATE, _ERR_FIRST_0, _ATO_ERR, 1, report.raw_first))
        {
            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _TSTATE, _ERR_DEBUG);
        }
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_ATO_ERR, "TS ATO timeout", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_TSTATE, _ERR_STATUS_0, _CAMRSP_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_TSTATE_CAMRSP_ERR, "Rsp Tag value out of range", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
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
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_TSTATE_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _TSTATE, _ERR_STATUS_0, pending);

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
_nvswitch_service_egress_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NvU32 pending_0, pending_1;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status = NVL_SUCCESS;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.egress[0].nonfatal;
    pending = report.raw_pending & report.mask;
    pending_0 = pending;

    if (pending == 0)
    {
        goto _nvswitch_service_egress_nonfatal_ls10_err_status_1;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FIRST_0);
    _nvswitch_collect_error_info_ls10(device, link,
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
            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_NXBAR_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, "egress input ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
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
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS,
                    _ERR_RAM_OUT_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_EGRESS_ERR_RAM_OUT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS,
                                                   _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER);
            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER, 0);
            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, "egress output ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_RAM_OUT_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_LIMIT_ERR, link, bAddressValid, address,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _PRIVRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_PRIVRSPERR, "egress non-posted PRIV error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_PRIVRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_EGRESS_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0, pending);

    // HACK: Clear all pending interrupts!
    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0, 0xFFFFFFFF);

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

_nvswitch_service_egress_nonfatal_ls10_err_status_1:
    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable & chip_device->intr_mask.egress[1].nonfatal;
    pending = report.raw_pending & report.mask;
    pending_1 = pending;

    if ((pending_0 == 0) && (pending_1 == 0))
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FIRST_1);

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_NXBAR_REDUCTION_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }


            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_NXBAR_REDUCTION_ECC_ERROR_COUNTER, 0);

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, "egress reduction header ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSPCTRLSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSPCTRLSTORE_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCRSPCTRLSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_LIMIT_ERR, "egress MC response ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBCTRLSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBCTRLSTORE_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_RBCTRLSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_LIMIT_ERR, "egress RB ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDSGT_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDSGT_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCREDSGT_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCREDSGT_ECC_LIMIT_ERR, "egress RSG ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCREDSGT_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDSGT_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDBUF_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDBUF_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCREDBUF_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCREDBUF_ECC_LIMIT_ERR, "egress MCRB ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCREDBUF_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDBUF_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_RAM_HDR_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_RAM_HDR_ECC_DBE_ERR, 1))))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCRSP_RAM_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_LIMIT_ERR, "egress MC header ECC error limit");
            NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_LIMIT_ERR, data);

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_LIMIT_ERR, link, NV_FALSE, 0,
                NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_DBE_ERR, "egress reduction header ECC DBE error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_NXBAR_REDUCTION_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_HDR_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_PARITY_ERR, "egress reduction header parity error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_REDUCTION_HDR_PARITY_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _NXBAR_REDUCTION_FLITTYPE_MISMATCH_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_NXBAR_REDUCTION_FLITTYPE_MISMATCH_ERR, "egress reduction flit mismatch error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_NXBAR_REDUCTION_FLITTYPE_MISMATCH_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDBUF_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCREDBUF_ECC_DBE_ERR, "egress reduction buffer ECC DBE error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCREDBUF_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDBUF_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDBUF_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCREDBUF_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDBUF_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_CNT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_HDR,
            &data);
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_MCRSP_CNT_ERR, "egress MC response count error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_MCRSP_CNT_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBRSP_CNT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MC_HDR,
            &data);
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_EGRESS_RBRSP_CNT_ERR, "egress reduction response count error");
        NVSWITCH_REPORT_DATA(_HW_NPORT_EGRESS_RBRSP_CNT_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_EGRESS_ERR_NON_FATAL_REPORT_EN_1,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FIRST_1,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1, pending);

    // Clear all pending interrupts!
    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1, 0xFFFFFFFF);

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

    return status;
}

static NvlStatus
_nvswitch_service_egress_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NvU32 pending_0, pending_1;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    NVSWITCH_RAW_ERROR_LOG_TYPE credit_data = {0, { 0 }};
    NVSWITCH_RAW_ERROR_LOG_TYPE buffer_data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status = NVL_SUCCESS;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.egress[0].fatal;
    pending = report.raw_pending & report.mask;
    pending_0 = pending;

    if (pending == 0)
    {
        goto _nvswitch_service_egress_fatal_ls10_err_status_1;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_ls10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
        &data);

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _EGRESSBUFERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_EGRESSBUFERR, "egress crossbar overflow", NV_TRUE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_EGRESSBUFERR, data);

        buffer_data.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS0);
        buffer_data.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS1);
        buffer_data.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS2);
        buffer_data.data[3] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS3);
        buffer_data.data[4] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS4);
        buffer_data.data[5] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS5);
        buffer_data.data[6] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS6);
        buffer_data.data[7] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _BUFFER_POINTERS7);
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

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_NXBAR_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS,
                _ERR_RAM_OUT_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_EGRESS_ERR_RAM_OUT, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS,
                                               _ERR_RAM_OUT_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, "egress output ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_RAM_OUT_HDR_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_RAM_OUT_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0,
                DRF_NUM(_EGRESS, _ERR_STATUS_0, _RAM_OUT_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NCISOCCREDITOVFL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NCISOCCREDITOVFL, "egress credit overflow", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOCCREDITOVFL, data);

        credit_data.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT0);
        credit_data.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT1);
        credit_data.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT2);
        credit_data.data[3] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT3);
        credit_data.data[4] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT4);
        credit_data.data[5] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT5);
        credit_data.data[6] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT6);
        credit_data.data[7] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT7);
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

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _URRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_DROPNPURRSPERR, "egress non-posted UR error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_DROPNPURRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _HWRSPERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_HWRSPERR, "egress non-posted HW error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_HWRSPERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_HDR_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, "egress control parity error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_NXBAR_HDR_PARITY_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NCISOC_CREDIT_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, "egress credit parity error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, data);

        credit_data.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT0);
        credit_data.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT1);
        credit_data.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT2);
        credit_data.data[3] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT3);
        credit_data.data[4] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT4);
        credit_data.data[5] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT5);
        credit_data.data[6] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT6);
        credit_data.data[7] = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _NCISOC_CREDIT7);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NCISOC_CREDIT_PARITY_ERR, credit_data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
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

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _NXBAR_SIDEBAND_PD_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_NXBAR_SIDEBAND_PD_PARITY_ERR, "egress crossbar SB parity", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_NXBAR_SIDEBAND_PD_PARITY_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_0, _INVALIDVCSET_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_INVALIDVCSET_ERR, "egress invalid VC set", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_INVALIDVCSET_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_EGRESS_0_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

_nvswitch_service_egress_fatal_ls10_err_status_1:
    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable & chip_device->intr_mask.egress[1].fatal;
    pending = report.raw_pending & report.mask;
    pending_1 = pending;

    if ((pending_0 == 0) && (pending_1 == 0))
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_FIRST_1);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _EGRESS, _ERR_CONTAIN_EN_1);

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSPCTRLSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_DBE_ERR, "egress MC response ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSPCTRLSTORE_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSPCTRLSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCRSPCTRLSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSPCTRLSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBCTRLSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_DBE_ERR, "egress reduction ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_RBCTRLSTORE_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBCTRLSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_RBCTRLSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _RBCTRLSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDSGT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_MCREDSGT_ECC_DBE_ERR, "egress MC SG ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_MCREDSGT_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_MCREDSGT_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDSGT_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCREDSGT_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCREDSGT_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_RAM_HDR_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_DBE_ERR, "egress MC ram ECC DBE error", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_EGRESS_MCRSP_RAM_HDR_ECC_DBE_ERR, link, NV_FALSE, 0,
            NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_RAM_HDR_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_EGRESS_ERR_MCRSP_RAM_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1,
                DRF_NUM(_EGRESS, _ERR_STATUS_1, _MCRSP_RAM_HDR_ECC_LIMIT_ERR, 1));
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_EGRESS_1_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FATAL_REPORT_EN_1,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_FIRST_1,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1, pending);

    // Clear all pending interrupts!
    NVSWITCH_ENG_WR32(device, NPORT, , link, _EGRESS, _ERR_STATUS_1, 0xFFFFFFFF);

    if (unhandled != 0)
    {
        status = -NVL_MORE_PROCESSING_REQUIRED;
    }

    return status;
}

static NvlStatus
_nvswitch_service_sourcetrack_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32           link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link,
                            _SOURCETRACK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link,
                            _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.sourcetrack.nonfatal;

    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK, _ERR_FIRST_0);

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if (!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                    _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                             _VALID, _VALID, addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                                                   _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER);
            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
            report.data[2] = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
            NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK,
                                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER, 0);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR,
                                    "sourcetrack TCEN0 crumbstore ECC limit err");

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, link,
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
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_SOURCETRACK_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_sourcetrack_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link,
                            _SOURCETRACK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link,
                            _SOURCETRACK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.sourcetrack.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID,
                         _VALID, _VALID, addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                                               _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[0] = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                            _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS);
        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _SOURCETRACK,
                            _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR,
                                "sourcetrack TCEN0 crumbstore DBE", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_SOURCETRACK_CREQ_TCEN0_CRUMBSTORE_ECC_DBE_ERR,
            link, bAddressValid, address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_STATUS_0,
                DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _CREQ_TCEN0_CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _DUP_CREQ_TCEN0_TAG_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_DUP_CREQ_TCEN0_TAG_ERR,
                                "sourcetrack duplicate CREQ", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _INVALID_TCEN0_RSP_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_INVALID_TCEN0_RSP_ERR,
                                "sourcetrack invalid TCEN0 CREQ", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_SOURCETRACK, _ERR_STATUS_0, _INVALID_TCEN1_RSP_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_SOURCETRACK_INVALID_TCEN1_RSP_ERR,
                                "sourcetrack invalid TCEN1 CREQ", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_SOURCETRACK_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _SOURCETRACK, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;

}

//
// Multicast Tstate
//

static NvlStatus
_nvswitch_service_multicast_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.mc_tstate.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FIRST_0);
    _nvswitch_collect_error_info_ls10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_MC_TIME,
        &data);

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                    _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_MULTICASTTSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                                                   _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_LIMIT_ERR, "MC TS tag store single-bit threshold");
            NVSWITCH_REPORT_DATA(_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_MULTICASTTSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                    _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_MULTICASTTSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                                                   _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, "MC TS crumbstore single-bit threshold");
            NVSWITCH_REPORT_DATA(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_MULTICASTTSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_MCTO_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_MCTO_ERR, "MC TS crumbstore MCTO");
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_DATA(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_MCTO_ERR, data);

        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_MULTICASTTSTATE_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_multicast_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.mc_tstate.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_ls10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_MC_TIME,
        &data);

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_MULTICASTTSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                                               _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
            DRF_DEF(_MULTICASTTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_DBE_ERR, "MC TS tag store fatal ECC", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_TAGPOOL_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_MULTICASTTSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0,
                DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_MULTICASTTSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE,
                                               _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
            DRF_DEF(_MULTICASTTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_DBE_ERR, "MC TS crumbstore fatal ECC", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_MULTICASTTSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0,
                DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_MULTICASTTSTATE, _ERR_STATUS_0, _CRUMBSTORE_BUF_OVERWRITE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR, "MC crumbstore overwrite", NV_FALSE);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_MULTICASTTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_MULTICAST_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _MULTICASTTSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

//
// Reduction Tstate
//

static NvlStatus
_nvswitch_service_reduction_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.mc_tstate.nonfatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FIRST_0);
    _nvswitch_collect_error_info_ls10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_RED_TIME,
        &data);

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                    _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_REDUCTIONTSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                                                   _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_LIMIT_ERR, "Red TS tag store single-bit threshold");
            NVSWITCH_REPORT_DATA(_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_REDUCTIONTSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Ignore LIMIT error if DBE is pending
        if(!(nvswitch_test_flags(report.raw_pending,
                DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1))))
        {
            NvBool bAddressValid = NV_FALSE;
            NvU32 address = 0;
            NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                    _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

            if (FLD_TEST_DRF(_REDUCTIONTSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                             addressValid))
            {
                address = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                                                   _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
                bAddressValid = NV_TRUE;
            }

            report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
            NVSWITCH_REPORT_NONFATAL(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, "Red TS crumbstore single-bit threshold");
            NVSWITCH_REPORT_DATA(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, data);

            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_REDUCTIONTSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            _nvswitch_construct_ecc_error_event_ls10(&err_event,
                NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_LIMIT_ERR, link,
                bAddressValid, address, NV_FALSE, 1);

            nvswitch_inforom_ecc_log_err_event(device, &err_event);
        }

        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_RTO_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_RTO_ERR, "Red TS crumbstore RTO");
        NVSWITCH_REPORT_DATA(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_RTO_ERR, data);

        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                         RM_SOE_CORE_ENGINE_ID_NPORT,
                                                         link, 
                                                         RM_SOE_CORE_NPORT_REDUCTIONTSTATE_ERR_NON_FATAL_REPORT_EN_0,
                                                         report.raw_enable & ~pending
                                                        );
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Disabling NPG[%d] non-fatal interrupts\n", __FUNCTION__, NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 
                                                                   NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_reduction_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NVSWITCH_RAW_ERROR_LOG_TYPE data = {0, { 0 }};
    INFOROM_NVS_ECC_ERROR_EVENT err_event = {0};
    NvlStatus status;

    report.raw_pending = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & chip_device->intr_mask.mc_tstate.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_CONTAIN_EN_0);
    _nvswitch_collect_error_info_ls10(device, link,
        NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_RED_TIME,
        &data);

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                _ERR_TAGPOOL_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_REDUCTIONTSTATE_ERR_TAGPOOL, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                                               _ERR_TAGPOOL_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
            DRF_DEF(_REDUCTIONTSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_DBE_ERR, "Red TS tag store fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_EGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_TAGPOOL_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_REDUCTIONTSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0,
                DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _TAGPOOL_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NvBool bAddressValid = NV_FALSE;
        NvU32 address = 0;
        NvU32 addressValid = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS_VALID);

        if (FLD_TEST_DRF(_REDUCTIONTSTATE_ERR_CRUMBSTORE, _ECC_ERROR_ADDRESS_VALID, _VALID, _VALID,
                         addressValid))
        {
            address = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE,
                                               _ERR_CRUMBSTORE_ECC_ERROR_ADDRESS);
            bAddressValid = NV_TRUE;
        }

        report.data[1] = NVSWITCH_ENG_RD32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER);
        NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
            DRF_DEF(_REDUCTIONTSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, _INIT));
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_DBE_ERR, "Red TS crumbstore fatal ECC", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_DBE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);

        _nvswitch_construct_ecc_error_event_ls10(&err_event,
            NVSWITCH_ERR_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_ECC_DBE_ERR, link, bAddressValid,
            address, NV_TRUE, 1);

        nvswitch_inforom_ecc_log_err_event(device, &err_event);

        // Clear associated LIMIT_ERR interrupt
        if (report.raw_pending & DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1))
        {
            status = nvswitch_soe_clear_engine_interrupt_counter_ls10(device,
                                                                      RM_SOE_CORE_ENGINE_ID_NPORT,
                                                                      RM_SOE_CORE_REDUCTIONTSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
                                                                      link
                                                                     );
            if(status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, INFO, "%s: NPORT counter clear command failed. Disabling NPG interrupt at GIN\n", __FUNCTION__);

                //
                // Driver WAR to disable NPG interrupt at GIN to prevent interrupt storm.
                //
                NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX),
                    NVBIT(link / NVSWITCH_LINKS_PER_NPG_LS10));
            }

            NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0,
                DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_ECC_LIMIT_ERR, 1));
        }
    }

    bit = DRF_NUM(_REDUCTIONTSTATE, _ERR_STATUS_0, _CRUMBSTORE_BUF_OVERWRITE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_CONTAIN(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR, "Red crumbstore overwrite", NV_FALSE);
        _nvswitch_collect_error_info_ls10(device, link,
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_TIME |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_MISC |
            NVSWITCH_RAW_ERROR_LOG_DATA_FLAG_INGRESS_HDR,
            &data);
        NVSWITCH_REPORT_CONTAIN_DATA(_HW_NPORT_REDUCTIONTSTATE_CRUMBSTORE_BUF_OVERWRITE_ERR, data);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    if (device->link[link].fatal_error_occurred)
    {
        if (nvswitch_is_soe_supported(device))
        {
            nvswitch_soe_disable_nport_fatal_interrupts_ls10(device, link, 
                report.raw_enable & ~pending, RM_SOE_CORE_NPORT_REDUCTION_INTERRUPT);
        }
        else
        {
            NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
        }
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_ENG_WR32(device, NPORT, , link, _REDUCTIONTSTATE, _ERR_STATUS_0, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nport_fatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    NvlStatus status[7];

    status[0] = _nvswitch_service_route_fatal_ls10(device, link);
    status[1] = _nvswitch_service_ingress_fatal_ls10(device, link);
    status[2] = _nvswitch_service_egress_fatal_ls10(device, link);
    status[3] = _nvswitch_service_tstate_fatal_ls10(device, link);
    status[4] = _nvswitch_service_sourcetrack_fatal_ls10(device, link);
    status[5] = _nvswitch_service_multicast_fatal_ls10(device, link);
    status[6] = _nvswitch_service_reduction_fatal_ls10(device, link);

    if ((status[0] != NVL_SUCCESS) &&
        (status[1] != NVL_SUCCESS) &&
        (status[2] != NVL_SUCCESS) &&
        (status[3] != NVL_SUCCESS) &&
        (status[4] != NVL_SUCCESS) &&
        (status[5] != NVL_SUCCESS) &&
        (status[6] != NVL_SUCCESS))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_npg_fatal_ls10
(
    nvswitch_device *device,
    NvU32            npg
)
{
    NvU32 pending, mask, bit, unhandled;
    NvU32 nport;
    NvU32 link;

    pending = NVSWITCH_ENG_RD32(device, NPG, , npg, _NPG, _NPG_INTERRUPT_STATUS);

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

    for (nport = 0; nport < NVSWITCH_NPORT_PER_NPG_LS10; nport++)
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
        }
        if (nvswitch_test_flags(pending, bit))
        {
            link = NPORT_TO_LINK_LS10(device, npg, nport);
            if (NVSWITCH_ENG_IS_VALID(device, NPORT, link))
            {
                if (_nvswitch_service_nport_fatal_ls10(device, link) == NVL_SUCCESS)
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
_nvswitch_service_nport_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32            link
)
{
    NvlStatus status[7];

    status[0] = _nvswitch_service_route_nonfatal_ls10(device, link);
    status[1] = _nvswitch_service_ingress_nonfatal_ls10(device, link);
    status[2] = _nvswitch_service_egress_nonfatal_ls10(device, link);
    status[3] = _nvswitch_service_tstate_nonfatal_ls10(device, link);
    status[4] = _nvswitch_service_sourcetrack_nonfatal_ls10(device, link);
    status[5] = _nvswitch_service_multicast_nonfatal_ls10(device, link);
    status[6] = _nvswitch_service_reduction_nonfatal_ls10(device, link);

    if ((status[0] != NVL_SUCCESS) &&
        (status[1] != NVL_SUCCESS) &&
        (status[2] != NVL_SUCCESS) &&
        (status[3] != NVL_SUCCESS) &&
        (status[4] != NVL_SUCCESS) &&
        (status[5] != NVL_SUCCESS) &&
        (status[6] != NVL_SUCCESS))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_npg_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 npg
)
{
    NvU32 pending, mask, bit, unhandled;
    NvU32 nport;
    NvU32 link;

    pending = NVSWITCH_ENG_RD32(device, NPG, , npg, _NPG, _NPG_INTERRUPT_STATUS);

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

    for (nport = 0; nport < NVSWITCH_NPORT_PER_NPG_LS10; nport++)
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
        }
        if (nvswitch_test_flags(pending, bit))
        {
            link = NPORT_TO_LINK_LS10(device, npg, nport);
            if (NVSWITCH_ENG_IS_VALID(device, NPORT, link))
            {
                if (_nvswitch_service_nport_nonfatal_ls10(device, link) == NVL_SUCCESS)
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
_nvswitch_service_nvldl_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU64 intrLinkMask
)
{
    NvU64 enabledLinkMask, localLinkMask, localIntrLinkMask, runtimeErrorMask = 0;
    NvU32 i;
    nvlink_link *link;
    NvU32 clocksMask = NVSWITCH_PER_LINK_CLOCK_SET(RXCLK) | NVSWITCH_PER_LINK_CLOCK_SET(TXCLK);
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;
    NVSWITCH_LINK_TRAINING_ERROR_INFO linkTrainingErrorInfo = { 0 };
    NVSWITCH_LINK_RUNTIME_ERROR_INFO linkRuntimeErrorInfo = { 0 };

    //
    // The passed in interruptLinkMask should contain a link that is part of the
    // given nvlipt instance
    //
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(nvlipt_instance);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
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

        if (nvswitch_is_link_in_reset(device, link) ||
            !nvswitch_are_link_clocks_on_ls10(device, link, clocksMask))
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

    // Check runtimeErrorMask is non-zero before consuming it further.
    if ((runtimeErrorMask != 0) &&
        (nvswitch_smbpbi_set_link_error_info(device,
            &linkTrainingErrorInfo, &linkRuntimeErrorInfo) != NVL_SUCCESS))
    {
        NVSWITCH_PRINT(device, ERROR,
                       "%s: NVLDL[0x%x, 0x%llx]: Unable to send Runtime Error bitmask: 0x%llx,\n",
                       __FUNCTION__,
                       nvlipt_instance, localIntrLinkMask,
                       runtimeErrorMask);
    }

    return status;
}

static NvlStatus
_nvswitch_service_nvltlc_tx_sys_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLTLC_TX_SYS, _ERR_STATUS_0, _NCISOC_PARITY_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TX_SYS_NCISOC_PARITY_ERR, "NCISOC Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0, _NCISOC_ECC_LIMIT_ERR, 1));
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
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
    }

    NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_SYS, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_rx_sys_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FIRST_0);

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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0, _HDR_RAM_ECC_LIMIT_ERR, 1));
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT0_RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT0_RAM_ECC_DBE_ERR, "DAT0 RAM ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0, _DAT0_RAM_ECC_LIMIT_ERR, 1));
    }

    bit = DRF_NUM(_NVLTLC_RX_SYS, _ERR_STATUS_0, _DAT1_RAM_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_SYS_DAT1_RAM_ECC_DBE_ERR, "DAT1 RAM ECC DBE Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0, _DAT1_RAM_ECC_LIMIT_ERR, 1));
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
    }

    NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_SYS, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_tx_lnk_fatal_0_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _TXDLCREDITPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_TXDLCREDITPARITYERR, "TX DL Credit Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_rx_lnk_fatal_0_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXDLHDRPARITYERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXDLHDRPARITYERR, "RX DL HDR Parity Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_DL_CTRL_PARITY_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXPKTLENERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RXPKTLENERR, "RX Packet Length Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_PKTLEN_ERR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RSVCACHEATTRPROBEREQERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RSVCACHEATTRPROBEREQERR, "RSV Packet Status Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_PO_FOR_CACHE_ATTR_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXRSPSTATUS_HW_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLTLC_RX_LNK_RXRSPSTATUS_HW_ERR, "RX Rsp Status HW Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_INVALID_COLLAPSED_RESPONSE_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_rx_lnk_fatal_1_ls10
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

    report.raw_pending = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1);
    injected = NVSWITCH_LINK_RD32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1);

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
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FATAL_REPORT_EN_1,
                report.raw_enable & ~pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1, pending);

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
_nvswitch_service_nvltlc_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU64 intrLinkMask
)
{
    NvU64 enabledLinkMask, localLinkMask, localIntrLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status = -NVL_MORE_PROCESSING_REQUIRED;

    //
    // The passed in interruptLinkMask should contain a link that is part of the
    // given nvlipt instance
    //
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(nvlipt_instance);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
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

        //
        // If link is in reset or NCISOC clock is off then
        // don't need to check the link for NVLTLC errors
        // as the IP's registers are off
        //
        if (nvswitch_is_link_in_reset(device, link) ||
            !nvswitch_are_link_clocks_on_ls10(device, link,NVSWITCH_PER_LINK_CLOCK_SET(NCISOCCLK)))
        {
            continue;
        }

        if (_nvswitch_service_nvltlc_tx_sys_fatal_ls10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_sys_fatal_ls10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_tx_lnk_fatal_0_ls10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_fatal_0_ls10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }

        if (_nvswitch_service_nvltlc_rx_lnk_fatal_1_ls10(device, nvlipt_instance, i) == NVL_SUCCESS)
        {
            status = NVL_SUCCESS;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

static NvlStatus
_nvswitch_service_nvlipt_common_fatal_ls10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, contain, unhandled;
    NvU32 link, local_link_idx;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_ENG_RD32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_ENG_RD32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable & (DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _CLKCTL_ILLEGAL_REQUEST, 1));

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    error_event.nvliptInstance = (NvU8) instance;

    unhandled = pending;
    report.raw_first = NVSWITCH_ENG_RD32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_FIRST_0);
    contain = NVSWITCH_ENG_RD32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_CONTAIN_EN_0);

    bit = DRF_NUM(_NVLIPT_COMMON, _ERR_STATUS_0, _CLKCTL_ILLEGAL_REQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT_LS10; local_link_idx++)
        {
            link = (instance * NVSWITCH_LINKS_PER_NVLIPT_LS10) + local_link_idx;
            if (nvswitch_is_link_valid(device, link))
            {
                NVSWITCH_REPORT_CONTAIN(_HW_NVLIPT_CLKCTL_ILLEGAL_REQUEST, "CLKCTL_ILLEGAL_REQUEST", NV_FALSE);
            }
        }

        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_CLKCTL_ILLEGAL_REQUEST_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    for (local_link_idx = 0; local_link_idx < NVSWITCH_LINKS_PER_NVLIPT_LS10; local_link_idx++)
    {
        link = (instance * NVSWITCH_LINKS_PER_NVLIPT_LS10) + local_link_idx;
        if (nvswitch_is_link_valid(device, link) &&
            (device->link[link].fatal_error_occurred))
        {
            NVSWITCH_ENG_WR32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
            break;
        }
    }

    // clear the interrupts
    if (report.raw_first & report.mask)
    {
        NVSWITCH_ENG_WR32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }
    NVSWITCH_ENG_WR32(device, NVLIPT, , instance, _NVLIPT_COMMON, _ERR_STATUS_0, pending);

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
_nvswitch_service_nxbar_tile_ls10
(
    nvswitch_device *device,
    NvU32 tile
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 link = tile;
    NvlStatus status;

    report.raw_pending = NVSWITCH_TILE_RD32(device, tile, _NXBAR_TILE, _ERR_STATUS);
    report.raw_enable = NVSWITCH_TILE_RD32(device, tile, _NXBAR_TILE, _ERR_FATAL_INTR_EN);
    report.mask = chip_device->intr_mask.tile.fatal;
    pending = report.raw_pending & report.mask;

   if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_TILE_RD32(device, tile, _NXBAR_TILE, _ERR_FIRST);

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
        NVSWITCH_TILE_WR32(device, tile, _NXBAR_TILE, _ERR_FIRST,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                     RM_SOE_CORE_ENGINE_ID_NXBAR_TILE,
                                                     link, 
                                                     RM_SOE_CORE_NXBAR_TILE_ERR_FATAL_INTR_EN,
                                                     report.raw_enable & ~pending
                                                    );

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: RM_SOE_CORE_NXBAR_TILE_ERR_FATAL_INTR_EN failed\n", __FUNCTION__);
    }

    NVSWITCH_TILE_WR32(device, link, _NXBAR_TILE, _ERR_STATUS, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nxbar_tileout_ls10
(
    nvswitch_device *device,
    NvU32 tileout
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 link = tileout;
    NvlStatus status;

    report.raw_pending = NVSWITCH_TILEOUT_RD32(device, tileout, _NXBAR_TILEOUT, _ERR_STATUS);
    report.raw_enable = NVSWITCH_TILEOUT_RD32(device, tileout, _NXBAR_TILEOUT, _ERR_FATAL_INTR_EN);
    report.mask = chip_device->intr_mask.tileout.fatal;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_TILEOUT_RD32(device, tileout, _NXBAR_TILEOUT, _ERR_FIRST);

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _INGRESS_BUFFER_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BUFFER_OVERFLOW, "ingress SRC-VC buffer overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _INGRESS_BUFFER_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BUFFER_UNDERFLOW, "ingress SRC-VC buffer underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _EGRESS_CREDIT_OVERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CREDIT_OVERFLOW, "egress DST-VC credit overflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _EGRESS_CREDIT_UNDERFLOW, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CREDIT_UNDERFLOW, "egress DST-VC credit underflow", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _INGRESS_NON_BURSTY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_NON_BURSTY_PKT, "ingress packet burst error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _INGRESS_NON_STICKY_PKT, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_NON_STICKY_PKT, "ingress packet sticky error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _INGRESS_BURST_GT_9_DATA_VC, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_INGRESS_BURST_GT_9_DATA_VC, "possible bubbles at ingress", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NXBAR_TILEOUT, _ERR_STATUS, _EGRESS_CDT_PARITY_ERROR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NXBAR_TILEOUT_EGRESS_CDT_PARITY_ERROR, "ingress credit parity error", NV_TRUE);
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_TILEOUT_WR32(device, tileout, _NXBAR_TILEOUT, _ERR_FIRST,
            report.raw_first & report.mask);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    // This helps prevent an interrupt storm if HW keeps triggering unnecessary stream of interrupts.
    status = nvswitch_soe_update_intr_report_en_ls10(device, 
                                                     RM_SOE_CORE_ENGINE_ID_NXBAR_TILEOUT,
                                                     link, 
                                                     RM_SOE_CORE_NXBAR_TILEOUT_ERR_FATAL_INTR_EN,
                                                     report.raw_enable & ~pending
                                                    );

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: RM_SOE_CORE_NXBAR_TILEOUT_ERR_FATAL_INTR_EN failed\n", __FUNCTION__);
    }

    NVSWITCH_TILEOUT_WR32(device, tileout, _NXBAR_TILEOUT, _ERR_STATUS, pending);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nxbar_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nxbar
)
{
    NvU32 pending, bit, unhandled;
    NvU32 tile_idx;
    NvU32 tile, tileout;

    pending = NVSWITCH_ENG_RD32(device, NXBAR, , nxbar, _NXBAR, _TCP_ERROR_STATUS);
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;

    for (tile = 0; tile < NUM_NXBAR_TILES_PER_TC_LS10; tile++)
    {
        bit = DRF_NUM(_NXBAR, _TCP_ERROR_STATUS, _TILE0, 1) << tile;
        if (nvswitch_test_flags(pending, bit))
        {
            tile_idx = TILE_INDEX_LS10(device, nxbar, tile);
            if (NVSWITCH_ENG_VALID_LS10(device, TILE, tile_idx))
            {
                if (_nvswitch_service_nxbar_tile_ls10(device, tile_idx) == NVL_SUCCESS)
                {
                    nvswitch_clear_flags(&unhandled, bit);
                }
            }
        }
    }

    for (tileout = 0; tileout < NUM_NXBAR_TILEOUTS_PER_TC_LS10; tileout++)
    {
        bit = DRF_NUM(_NXBAR, _TCP_ERROR_STATUS, _TILEOUT0, 1) << tileout;
        if (nvswitch_test_flags(pending, bit))
        {
            tile_idx = TILE_INDEX_LS10(device, nxbar, tileout);
            if (NVSWITCH_ENG_VALID_LS10(device, TILEOUT, tile_idx))
            {
                if (_nvswitch_service_nxbar_tileout_ls10(device, tile_idx) == NVL_SUCCESS)
                {
                    nvswitch_clear_flags(&unhandled, bit);
                }
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

static void
_nvswitch_emit_link_errors_nvldl_fatal_link_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit;
    INFOROM_NVLINK_ERROR_EVENT error_event;

    // Only enabled link errors are deffered
    pending = chip_device->deferredLinkErrors[link].data.fatalIntrMask.dl;
    report.raw_pending = pending;
    report.raw_enable = pending;
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_UP, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_FAULT_UP, "LTSSM Fault Up", NV_FALSE);
        error_event.error = INFOROM_NVLINK_DL_LTSSM_FAULT_UP_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_DOWN, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_FAULT_DOWN, "LTSSM Fault Down", NV_FALSE);
    }
}

static void
_nvswitch_dump_minion_ali_debug_registers_ls10
(
    nvswitch_device *device,
    NvU32 link
)
{
    NVSWITCH_MINION_ALI_DEBUG_REGISTERS params;
    nvlink_link *nvlink = nvswitch_get_link(device, link);

    if ((nvlink != NULL) &&
        (nvswitch_minion_get_ali_debug_registers_ls10(device, nvlink, &params) == NVL_SUCCESS))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Minion error on link #%d!:\n"
                "Minion DLSTAT MN00 = 0x%x\n"
                "Minion DLSTAT UC01 = 0x%x\n"
                "Minion DLSTAT UC01 = 0x%x\n",
            __FUNCTION__, link,
            params.dlstatMn00, params.dlstatUc01, params.dlstatLinkIntr);
    }
}

static void
_nvswitch_emit_link_errors_minion_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 regData;
    NvU32 enabledLinks;
    NvU32 localLinkIdx = NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);
    NvU32 bit = BIT(localLinkIdx);

    if (!chip_device->deferredLinkErrors[link].data.fatalIntrMask.minionLinkIntr.bPending)
    {
        return;
    }

    // Grab the cached interrupt data
    regData     =  chip_device->deferredLinkErrors[link].data.fatalIntrMask.minionLinkIntr.regData;

    // get all possible interrupting links associated with this minion
    report.raw_enable  = link;
    report.raw_pending = report.raw_enable;
    report.mask = report.raw_enable;
    report.data[0] = regData;

    switch(DRF_VAL(_MINION, _NVLINK_LINK_INTR, _CODE, regData))
    {
        case NV_MINION_NVLINK_LINK_INTR_CODE_NA:
            NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link NA interrupt", NV_FALSE);
            break;
        case NV_MINION_NVLINK_LINK_INTR_CODE_DLCMDFAULT:
            NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link DLCMDFAULT interrupt", NV_FALSE);
            break;
        case NV_MINION_NVLINK_LINK_INTR_CODE_NOINIT:
            NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Link NOINIT interrupt", NV_FALSE);
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

    // Fatal error was hit so disable the interrupt
    regData = NVSWITCH_MINION_RD32_LS10(device, nvlipt_instance, _MINION, _MINION_INTR_STALL_EN);
    enabledLinks = DRF_VAL(_MINION, _MINION_INTR_STALL_EN, _LINK, regData);
    enabledLinks &= ~bit;
    regData = DRF_NUM(_MINION, _MINION_INTR_STALL_EN, _LINK, enabledLinks);
    NVSWITCH_MINION_LINK_WR32_LS10(device, link, _MINION, _MINION_INTR_STALL_EN, regData);

    _nvswitch_dump_minion_ali_debug_registers_ls10(device, link);
}

static void
_nvswitch_emit_link_errors_minion_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 regData;
    NvU32 localLinkIdx = NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);
    NvU32 bit = BIT(localLinkIdx);

    if (!chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.minionLinkIntr.bPending)
    {
        return;
    }

    // read in the enaled minion interrupts on this minion
    regData = NVSWITCH_MINION_RD32_LS10(device, nvlipt_instance, _MINION, _MINION_INTR_STALL_EN);

    // Grab the cached interrupt data
    regData     =  chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.minionLinkIntr.regData;

    // get all possible interrupting links associated with this minion
    report.raw_enable  = link;
    report.raw_pending = report.raw_enable;
    report.mask = report.raw_enable;
    report.data[0] = regData;

    switch(DRF_VAL(_MINION, _NVLINK_LINK_INTR, _CODE, regData))
    {
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
    }

    _nvswitch_dump_minion_ali_debug_registers_ls10(device, link);
}

static void
_nvswitch_emit_link_errors_nvldl_nonfatal_link_ls10
(
    nvswitch_device *device,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, reg;

    // Only enabled link errors are deffered
    pending = chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.dl;
    report.raw_pending = pending;
    report.raw_enable = pending;
    report.mask = report.raw_enable;

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_SHORT_ERROR_RATE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        // Disable further interrupts
        nvlink_link *nvlink = nvswitch_get_link(device, link);
        if (nvlink == NULL)
        {
            // If we get here, it is a bug. Disable interrupt and assert.
            reg = NVSWITCH_LINK_RD32_LS10(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN);
            reg = FLD_SET_DRF_NUM(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, 0, reg);
            NVSWITCH_LINK_WR32_LS10(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN, reg);
            NVSWITCH_ASSERT(nvlink != NULL);
        }
        else
        {
            nvlink->errorThreshold.bInterruptTrigerred = NV_TRUE;
            nvswitch_configure_error_rate_threshold_interrupt_ls10(nvlink, NV_FALSE);
            NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_SHORT_ERROR_RATE, "RX Short Error Rate");
        }
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_CRC_COUNTER, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_DLPL_RX_CRC_COUNTER, "RX CRC Error Rate");
    }
}

static void
_nvswitch_emit_link_errors_nvltlc_rx_lnk_nonfatal_1_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit;
    INFOROM_NVLINK_ERROR_EVENT error_event;
    NvU32 injected;

    // Only enabled link errors are deffered
    pending = chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.tlcRx1;
    injected = chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.tlcRx1Injected;
    report.raw_pending = pending;
    report.raw_enable = pending;
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);


    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _HEARTBEAT_TIMEOUT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_RX_LNK_AN1_HEARTBEAT_TIMEOUT_ERR, "AN1 Heartbeat Timeout Error");

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1, _HEARTBEAT_TIMEOUT_ERR, 0x0, injected))
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_AN1_HEARTBEAT_TIMEOUT_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }
}

static void
_nvswitch_emit_link_errors_nvlipt_lnk_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit;
    INFOROM_NVLINK_ERROR_EVENT error_event;

    // Only enabled link errors are deffered
    pending = chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.liptLnk;
    report.raw_pending = pending;
    report.raw_enable = pending;
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _FAILEDMINIONREQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_FAILEDMINIONREQUEST, "_FAILEDMINIONREQUEST");

        {
            error_event.error = INFOROM_NVLINK_NVLIPT_FAILED_MINION_REQUEST_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }
}

static void
_nvswitch_emit_deferred_link_errors_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    _nvswitch_emit_link_errors_nvldl_fatal_link_ls10(device, nvlipt_instance, link);
    _nvswitch_emit_link_errors_nvldl_nonfatal_link_ls10(device, link);
    _nvswitch_emit_link_errors_nvltlc_rx_lnk_nonfatal_1_ls10(device, nvlipt_instance, link);
    _nvswitch_emit_link_errors_nvlipt_lnk_nonfatal_ls10(device, nvlipt_instance, link);
    _nvswitch_emit_link_errors_minion_fatal_ls10(device, nvlipt_instance, link);
    _nvswitch_emit_link_errors_minion_nonfatal_ls10(device, nvlipt_instance, link);

}

static void
_nvswitch_clear_deferred_link_errors_ls10
(
    nvswitch_device *device,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVLINK_LINK_ERROR_REPORTING_DATA  *pLinkErrorsData;

    pLinkErrorsData  = &chip_device->deferredLinkErrors[link].data;

    nvswitch_os_memset(pLinkErrorsData, 0, sizeof(NVLINK_LINK_ERROR_REPORTING_DATA));
}

static void
_nvswitch_deferred_link_state_check_ls10
(
    nvswitch_device *device,
    void *fn_args
)
{
    NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS *pErrorReportParams =
                                           (NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS*)fn_args;
    NvU32 nvlipt_instance = pErrorReportParams->nvlipt_instance;
    NvU32 link = pErrorReportParams->link;
    ls10_device *chip_device;
    NvU64 lastLinkUpTime;
    NvU64 lastRetrainTime;
    NvU64 current_time = nvswitch_os_get_platform_time();
    NvBool bRedeferLinkStateCheck;

    chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    lastLinkUpTime = chip_device->deferredLinkErrors[link].state.lastLinkUpTime;
    lastRetrainTime = chip_device->deferredLinkErrors[link].state.lastRetrainTime;
    // Sanity Check
    if (!nvswitch_is_link_valid(device, link))
        return;

    chip_device->deferredLinkErrors[link].state.bLinkStateCallBackEnabled = NV_FALSE;
    bRedeferLinkStateCheck = NV_FALSE;

    // Ask CCI if link state check should be futher deferred 
    if (cciIsLinkManaged(device, link) && !cciReportLinkErrors(device, link))
    {
        bRedeferLinkStateCheck = NV_TRUE;
    }

    // Link came up after last retrain
    if (lastLinkUpTime >= lastRetrainTime)
    {
        return;
    }

    //
    // If the last time this link was up was before the last
    // reset_and_drain execution and not enough time has past since the last
    // retrain then schedule another callback.
    //
    if (lastLinkUpTime < lastRetrainTime)
    {
        if ((current_time - lastRetrainTime) < NVSWITCH_DEFERRED_LINK_STATE_CHECK_INTERVAL_NS)
        {
            bRedeferLinkStateCheck = NV_TRUE;
        }
    }

    if (bRedeferLinkStateCheck)
    {
        nvswitch_create_deferred_link_state_check_task_ls10(device, nvlipt_instance, link);
        return;
    }

    //
    // Otherwise, the link hasn't retrained within the timeout so emit the
    // deferred errors.
    //
        _nvswitch_emit_deferred_link_errors_ls10(device, nvlipt_instance, link);
    _nvswitch_clear_deferred_link_errors_ls10(device, link);
}

void
nvswitch_create_deferred_link_state_check_task_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS *pErrorReportParams;
    NvlStatus status;

    if (chip_device->deferredLinkErrors[link].state.bLinkStateCallBackEnabled)
    {
        return;
    }

    status = NVL_ERR_GENERIC;
    pErrorReportParams = &chip_device->deferredLinkErrorsArgs[link];
    nvswitch_os_memset(pErrorReportParams, 0, sizeof(NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS));
 
    pErrorReportParams->nvlipt_instance = nvlipt_instance;
    pErrorReportParams->link = link;

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        status = nvswitch_task_create_args(device, (void*)pErrorReportParams,
                                            &_nvswitch_deferred_link_state_check_ls10,
                                            NVSWITCH_DEFERRED_LINK_STATE_CHECK_INTERVAL_NS,
                                            NVSWITCH_TASK_TYPE_FLAGS_RUN_ONCE |
                                            NVSWITCH_TASK_TYPE_FLAGS_VOID_PTR_ARGS);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "Skipping Deferred link state background task when TNVL is enabled\n");
    }

    if (status == NVL_SUCCESS)
    {
        chip_device->deferredLinkErrors[link].state.bLinkStateCallBackEnabled = NV_TRUE;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
                        "%s: Failed to allocate memory. Cannot defer link state check.\n",
                        __FUNCTION__);
        _nvswitch_emit_deferred_link_errors_ls10(device, nvlipt_instance, link);
        _nvswitch_clear_deferred_link_errors_ls10(device, link);
    }
}

static void
_nvswitch_deferred_link_errors_check_ls10
(
    nvswitch_device *device,
    void *fn_args
)
{
    NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS *pErrorReportParams =
                                           (NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS*)fn_args;
    NvU32 nvlipt_instance = pErrorReportParams->nvlipt_instance;
    NvU32 link = pErrorReportParams->link;
    ls10_device *chip_device;
    NvU32 pending;

    chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    chip_device->deferredLinkErrors[link].state.bLinkErrorsCallBackEnabled = NV_FALSE;

    pending = chip_device->deferredLinkErrors[link].data.fatalIntrMask.dl;

    // A link fault was observed which means we also did the retrain and
    // scheduled a state check task. We can exit.
    if (FLD_TEST_DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_UP, 1U, pending))
        return;

    if (FLD_TEST_DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_DOWN, 1U, pending))
        return;

    //
    // No link fault, emit the deferred errors.
    // It is assumed that this callback runs long before a link could have been
    // retrained and hit errors again.
    //
    _nvswitch_emit_deferred_link_errors_ls10(device, nvlipt_instance, link);
    _nvswitch_clear_deferred_link_errors_ls10(device, link);
}

static void
_nvswitch_create_deferred_link_errors_task_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS *pErrorReportParams;
    NvlStatus status;

    if (chip_device->deferredLinkErrors[link].state.bLinkErrorsCallBackEnabled)
    {
        return;
    }

    status = NVL_ERR_GENERIC;
    pErrorReportParams = &chip_device->deferredLinkErrorsArgs[link];
    nvswitch_os_memset(pErrorReportParams, 0, sizeof(NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS));
    
    pErrorReportParams->nvlipt_instance = nvlipt_instance;
    pErrorReportParams->link = link;

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        status = nvswitch_task_create_args(device, (void*)pErrorReportParams,
                                            &_nvswitch_deferred_link_errors_check_ls10,
                                            NVSWITCH_DEFERRED_FAULT_UP_CHECK_INTERVAL_NS,
                                            NVSWITCH_TASK_TYPE_FLAGS_RUN_ONCE |
                                            NVSWITCH_TASK_TYPE_FLAGS_VOID_PTR_ARGS);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "Skipping Deferred link error checks background task when TNVL is enabled\n");
    }
   

    if (status == NVL_SUCCESS)
    {
        chip_device->deferredLinkErrors[link].state.bLinkErrorsCallBackEnabled = NV_TRUE;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
                        "%s: Failed to create task. Cannot defer link error check.\n",
                        __FUNCTION__);
        _nvswitch_emit_deferred_link_errors_ls10(device, nvlipt_instance, link);
        _nvswitch_clear_deferred_link_errors_ls10(device, link);
    }
}

static NvlStatus
_nvswitch_service_nvldl_nonfatal_link_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLDL, _NVLDL_TOP, _INTR);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN);
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
        chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.dl |= bit;
        _nvswitch_create_deferred_link_errors_task_ls10(device, nvlipt_instance, link);
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

        chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.dl |= bit;
        _nvswitch_create_deferred_link_errors_task_ls10(device, nvlipt_instance, link);
        nvswitch_clear_flags(&unhandled, bit);

        //
        // Mask CRC counter after first occurrance - otherwise, this interrupt
        // will continue to fire once the CRC counter has hit the threshold
        // See Bug 3341528
        //
        report.raw_enable = report.raw_enable & (~bit);
        NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN,
            report.raw_enable);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR, pending);

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
_nvswitch_service_nvldl_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU64 intrLinkMask
)
{
    NvU64 localLinkMask, enabledLinkMask, localIntrLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status;
    NvlStatus return_status = -NVL_NOT_FOUND;
    NvU32 clocksMask = NVSWITCH_PER_LINK_CLOCK_SET(RXCLK) | NVSWITCH_PER_LINK_CLOCK_SET(TXCLK);

    //
    // The passed in interruptLinkMask should contain a link that is part of the
    // given nvlipt instance
    //
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(nvlipt_instance);
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
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

        if (nvswitch_is_link_in_reset(device, link) ||
            !nvswitch_are_link_clocks_on_ls10(device, link, clocksMask))
        {
            continue;
        }

        status = _nvswitch_service_nvldl_nonfatal_link_ls10(device, nvlipt_instance, i);
        if (status != NVL_SUCCESS)
        {
            return_status = status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return return_status;
}

static NvlStatus
_nvswitch_service_nvltlc_rx_lnk_nonfatal_0_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_0, _RXRSPSTATUS_PRIV_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_RX_LNK_RXRSPSTATUS_PRIV_ERR, "RX Rsp Status PRIV Error");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_RX_RSP_STATUS_PRIV_ERR_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_tx_lnk_nonfatal_0_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _CREQ_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_CREQ_RAM_DAT_ECC_DBE_ERR, "CREQ RAM DAT ECC DBE Error");
        nvswitch_clear_flags(&unhandled, bit);
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0, _CREQ_RAM_ECC_LIMIT_ERR, 1));
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0, _RSP_RAM_ECC_LIMIT_ERR, 1));
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _COM_RAM_DAT_ECC_DBE_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_COM_RAM_DAT_ECC_DBE_ERR, "COM RAM DAT ECC DBE Error");
        nvswitch_clear_flags(&unhandled, bit);
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

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0, _COM_RAM_ECC_LIMIT_ERR, 1));
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_0, _RSP1_RAM_ECC_LIMIT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_RSP1_RAM_ECC_LIMIT_ERR, "RSP1 RAM ECC Limit Error");
        nvswitch_clear_flags(&unhandled, bit);

        //
        // Driver WAR to disable ECC error and prevent an interrupt storm.
        // TODO: Clear ECC_ERROR_COUNTER by sending command to SOE and remove the WAR.
        //
        NVSWITCH_LINK_WR32_LS10(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable &
            ~DRF_NUM(_NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0, _RSP1_RAM_ECC_LIMIT_ERR, 1));
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_0,
            report.raw_enable & ~pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvltlc_rx_lnk_nonfatal_1_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 injected;

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1);
    injected = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1);

    bit = DRF_NUM(_NVLTLC_RX_LNK, _ERR_STATUS_1, _HEARTBEAT_TIMEOUT_ERR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.tlcRx1 |= bit;
        chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.tlcRx1Injected |= injected;
        _nvswitch_create_deferred_link_errors_task_ls10(device, nvlipt_instance, link);

        if (FLD_TEST_DRF_NUM(_NVLTLC_RX_LNK, _ERR_REPORT_INJECT_1, _HEARTBEAT_TIMEOUT_ERR, 0x0, injected))
        {
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
            NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
                report.raw_enable);
        }
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
            report.raw_enable & (~pending));
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_RX_LNK, _ERR_STATUS_1, pending);

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
_nvswitch_service_nvltlc_tx_lnk_nonfatal_1_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NvU32 pending, bit, unhandled;
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_1);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_1);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_1);

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC0, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC0, "AN1 Timeout VC0");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC0_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC1, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC1, "AN1 Timeout VC1");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC1_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC2, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC2, "AN1 Timeout VC2");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC2_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC3, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC3, "AN1 Timeout VC3");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC3_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC4, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC4, "AN1 Timeout VC4");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC4_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC5, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC5, "AN1 Timeout VC5");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC5_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC6, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC6, "AN1 Timeout VC6");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC6_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLTLC_TX_LNK, _ERR_STATUS_1, _TIMEOUT_VC7, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLTLC_TX_LNK_AN1_TIMEOUT_VC7, "AN1 Timeout VC7");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_TLC_TX_AN1_TIMEOUT_VC7_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_NON_FATAL_REPORT_EN_1,
                report.raw_enable & ~pending);
    }

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_FIRST_1,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLTLC, _NVLTLC_TX_LNK, _ERR_STATUS_1, pending);

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
_nvswitch_service_nvltlc_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU64 intrLinkMask
)
{
    NvU64 localLinkMask, enabledLinkMask, localIntrLinkMask;
    NvU32 i;
    nvlink_link *link;
    NvlStatus status;
    NvlStatus return_status = NVL_SUCCESS;

    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(nvlipt_instance);
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
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

        //
        // If link is in reset or NCISOC clock is off then
        // don't need to check the link for NVLTLC errors
        // as the IP's registers are off
        //
        if (nvswitch_is_link_in_reset(device, link) ||
            !nvswitch_are_link_clocks_on_ls10(device, link, NVSWITCH_PER_LINK_CLOCK_SET(NCISOCCLK)))
        {
            continue;
        }

        status = _nvswitch_service_nvltlc_rx_lnk_nonfatal_0_ls10(device, nvlipt_instance, i);
        if (status != NVL_SUCCESS)
        {
            return_status = status;
        }

        status = _nvswitch_service_nvltlc_tx_lnk_nonfatal_0_ls10(device, nvlipt_instance, i);
        if (status != NVL_SUCCESS)
        {
            return_status = status;
        }

        status = _nvswitch_service_nvltlc_rx_lnk_nonfatal_1_ls10(device, nvlipt_instance, i);
        if (status != NVL_SUCCESS)
        {
            return_status = status;
        }

        status = _nvswitch_service_nvltlc_tx_lnk_nonfatal_1_ls10(device, nvlipt_instance, i);
        if (status != NVL_SUCCESS)
        {
            return_status = status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return return_status;
}

static NvlStatus
_nvswitch_service_nvlipt_lnk_status_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link_id
)
{
    NvU32 pending, enabled, unhandled, bit;
    NvU64 mode;
    nvlink_link *link;
    ls10_device *chip_device;

    link = nvswitch_get_link(device, link_id);
    chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    if (link == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    pending =  NVSWITCH_LINK_RD32(device, link_id, NVLIPT_LNK, _NVLIPT_LNK, _INTR_STATUS);
    enabled =  NVSWITCH_LINK_RD32(device, link_id, NVLIPT_LNK, _NVLIPT_LNK, _INTR_INT1_EN);
    pending &= enabled;
    unhandled = pending;

    bit = DRF_NUM(_NVLIPT_LNK, _INTR_STATUS, _LINKSTATEREQUESTREADYSET, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        link = nvswitch_get_link(device, link_id);
        if (link == NULL)
        {
            // If we get here, it's a bug. Assert, then let callers detect unhandled IRQ.
            NVSWITCH_ASSERT(link != NULL);
        }

        nvswitch_clear_flags(&unhandled, bit);
        if(nvswitch_corelib_get_dl_link_mode_ls10(link, &mode) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: nvlipt_lnk_status: Failed to check link mode! LinkId %d\n",
                        __FUNCTION__, link_id);
        }
        else if(mode == NVLINK_LINKSTATE_HS)
        {
            NVSWITCH_PRINT(device, INFO, "%s: nvlipt_lnk_status: Link is up!. LinkId %d\n",
                        __FUNCTION__, link_id);

            //
            // When a link comes up ensure that we finish off the post-training tasks:
            // -- enabling per-link DL interrupts
            // -- releasing buffer_ready on the link
            //
            nvswitch_corelib_training_complete_ls10(link);
            nvswitch_init_buffer_ready(device, link, NV_TRUE);
                link->bRxDetected = NV_TRUE;

            //
            // Clear out any cached interrupts for the link and update the last link up timestamp
            //
            _nvswitch_clear_deferred_link_errors_ls10(device, link_id);
            chip_device->deferredLinkErrors[link_id].state.lastLinkUpTime = nvswitch_os_get_platform_time();
        
            // Reset NV_NPORT_SCRATCH_WARM_PORT_RESET_REQUIRED to 0x0
            NVSWITCH_LINK_WR32(device, link_id, NPORT, _NPORT, _SCRATCH_WARM, 0);
        }
        else if (mode == NVLINK_LINKSTATE_FAULT)
        {
            //
            // If we are here then a previous state transition caused
            // the link to FAULT as there is no TL Link state requests
            // that explicitly transitions a link to fault. If that is the
            // case, set the DL interrupts so any errors can be handled
            //
            nvswitch_set_dlpl_interrupts_ls10(link);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);
    NVSWITCH_LINK_WR32(device, link_id, NVLIPT_LNK, _NVLIPT_LNK, _INTR_STATUS, pending);

    if (unhandled != 0)
    {
        NVSWITCH_PRINT(device, WARN,
                       "%s: Unhandled NVLIPT_LNK STATUS interrupts, pending: 0x%x enabled: 0x%x.\n",
                       __FUNCTION__, pending, enabled);
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlipt_lnk_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_NON_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _ILLEGALLINKSTATEREQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_ILLEGALLINKSTATEREQUEST, "_HW_NVLIPT_LNK_ILLEGALLINKSTATEREQUEST");
        nvswitch_clear_flags(&unhandled, bit);
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_ILLEGAL_LINK_STATE_REQUEST_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _FAILEDMINIONREQUEST, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        //
        // based off of HW's assertion. FAILEDMINIONREQUEST always trails a DL fault. So no need to
        // do reset_and_drain here
        //
        chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.liptLnk |= bit;
        _nvswitch_create_deferred_link_errors_task_ls10(device, nvlipt_instance, link);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _RESERVEDREQUESTVALUE, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_NONFATAL(_HW_NVLIPT_LNK_RESERVEDREQUESTVALUE, "_RESERVEDREQUESTVALUE");
        nvswitch_clear_flags(&unhandled, bit);
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
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_WRITE_TO_LOCKED_SYSTEM_REG_NONFATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0,
            report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvlipt_link_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 instance,
    NvU64 intrLinkMask
)
{
    NvU32 i, intrLink;
    NvU64 localLinkMask, enabledLinkMask, localIntrLinkMask;
    NvU64 interruptingLinks = 0;
    NvU64 lnkStatusChangeLinks = 0;
    NvlStatus status = NVL_SUCCESS;
    NvlStatus retStatus = NVL_SUCCESS;

    //
    // The passed in interruptLinkMask should contain a link that is part of the
    // given nvlipt instance
    //
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(instance);
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }


    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
    {
        if (NVSWITCH_GET_LINK_ENG_INST(device, i, NVLIPT) != instance)
        {
            NVSWITCH_ASSERT(0);
            break;
        }

        intrLink = NVSWITCH_LINK_RD32(device, i, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);

        if(intrLink)
        {
            interruptingLinks |= NVBIT64(i);
        }

       intrLink = NVSWITCH_LINK_RD32(device, i, NVLIPT_LNK, _NVLIPT_LNK, _INTR_STATUS);

        if(intrLink)
        {
            lnkStatusChangeLinks |= NVBIT64(i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;


    FOR_EACH_INDEX_IN_MASK(64, i, lnkStatusChangeLinks)
    {

        if(_nvswitch_service_nvlipt_lnk_status_ls10(device, instance, i) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, WARN, "%s: Could not process nvlipt link status interrupt. Continuing. LinkId %d\n",
                    __FUNCTION__, i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    FOR_EACH_INDEX_IN_MASK(64, i, interruptingLinks)
    {

        status = _nvswitch_service_nvlipt_lnk_nonfatal_ls10(device, instance, i);
        if (status != NVL_SUCCESS && status != -NVL_NOT_FOUND)
        {
            retStatus = -NVL_MORE_PROCESSING_REQUIRED;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return retStatus;
}


NvlStatus
_nvswitch_service_minion_fatal_ls10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvU32 pending, bit, unhandled, mask;

    pending = NVSWITCH_MINION_RD32_LS10(device, instance, _MINION, _MINION_INTR);
    mask =  NVSWITCH_MINION_RD32_LS10(device, instance, _MINION, _MINION_INTR_STALL_EN);

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
        if (nvswitch_minion_service_falcon_interrupts_ls10(device, instance) == NVL_SUCCESS)
        {
            nvswitch_clear_flags(&unhandled, bit);
        }
    }

    bit =  DRF_NUM(_MINION, _MINION_INTR, _NONFATAL, 0x1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: servicing minion nonfatal interrupt\n",
         __FUNCTION__);
        NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _MINION_INTR, bit);
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit =  DRF_NUM(_MINION, _MINION_INTR, _FATAL, 0x1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: servicing minion fatal interrupt\n",
         __FUNCTION__);
        NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _MINION_INTR, bit);
        nvswitch_clear_flags(&unhandled, bit);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlw_nonfatal_ls10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvlStatus status[3];
    NvU32 reg;
    NvU64 intrLinkMask = 0;

    reg = NVSWITCH_ENG_RD32_LS10(device, NVLW, instance, _NVLW, _TOP_INTR_1_STATUS);
    intrLinkMask = DRF_VAL(_NVLW, _TOP_INTR_1_STATUS, _LINK, reg);

    //
    // Shift the mask of interrupting links from the local to the
    // NVLW instance to a global mask
    //
    intrLinkMask = intrLinkMask << (NVSWITCH_LINKS_PER_NVLW_LS10*instance);

    // If there is no pending link interrupts then there is nothing to service
    if (intrLinkMask == 0)
    {
        return NVL_SUCCESS;
    }

    status[0] = _nvswitch_service_nvlipt_link_nonfatal_ls10(device, instance, intrLinkMask);
    status[1] = _nvswitch_service_nvldl_nonfatal_ls10(device, instance, intrLinkMask);
    status[2] = _nvswitch_service_nvltlc_nonfatal_ls10(device, instance, intrLinkMask);

    if ((status[0] != NVL_SUCCESS) && (status[0] != -NVL_NOT_FOUND) &&
        (status[1] != NVL_SUCCESS) && (status[1] != -NVL_NOT_FOUND) &&
        (status[2] != NVL_SUCCESS) && (status[2] != -NVL_NOT_FOUND))
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_soe_fatal_ls10
(
    nvswitch_device *device
)
{
    // We only support 1 SOE as of LS10.
    if (soeService_HAL(device, (PSOE)device->pSoe) != NVL_SUCCESS)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_service_nvlipt_lnk_fatal_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, bit, unhandled;
    INFOROM_NVLINK_ERROR_EVENT error_event = { 0 };

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FATAL_REPORT_EN_0);
    report.mask = report.raw_enable;

    pending = report.raw_pending & report.mask;
    if (pending == 0)
    {
        return -NVL_NOT_FOUND;
    }

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

    unhandled = pending;
    report.raw_first = NVSWITCH_LINK_RD32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0);

    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _SLEEPWHILEACTIVELINK, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_NVLIPT_LNK_SLEEPWHILEACTIVELINK, "No non-empty link is detected", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
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
        {
            error_event.error = INFOROM_NVLINK_NVLIPT_RSTSEQ_CLKCTL_TIMEOUT_FATAL;
            nvswitch_inforom_nvlink_log_error_event(device, &error_event);
        }
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FATAL_REPORT_EN_0,
                report.raw_enable & ~pending);
    }

    // clear interrupts
    if (report.raw_first & report.mask)
    {
        NVSWITCH_LINK_WR32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_FIRST_0,
                report.raw_first & report.mask);
    }
    NVSWITCH_LINK_WR32(device, link, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0, pending);

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
_nvswitch_service_nvlipt_link_fatal_ls10
(
    nvswitch_device *device,
    NvU32 instance,
    NvU64 intrLinkMask
)
{
    NvU32 i, intrLink;
    NvU64 localLinkMask, enabledLinkMask, localIntrLinkMask;
    NvlStatus status = NVL_SUCCESS;

    //
    // The passed in interruptLinkMask should contain a link that is part of the
    // given nvlipt instance
    //
    localLinkMask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64_LS10(instance);
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    localIntrLinkMask = localLinkMask & intrLinkMask & enabledLinkMask;

    if (localIntrLinkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad link mask provided for link interrupt servicing!\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    // read in error status of current link
    FOR_EACH_INDEX_IN_MASK(64, i, localIntrLinkMask)
    {
        intrLink = NVSWITCH_LINK_RD32(device, i, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
        if (intrLink != 0)
        {
            if( _nvswitch_service_nvlipt_lnk_fatal_ls10(device, instance, i) != NVL_SUCCESS)
            {
                status = -NVL_MORE_PROCESSING_REQUIRED;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

static NvlStatus
_nvswitch_service_nvlw_fatal_ls10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NvlStatus status[6];
    NvU64 intrLinkMask = 0;
    NvU32 reg;

    reg = NVSWITCH_ENG_RD32_LS10(device, NVLW, instance, _NVLW, _TOP_INTR_0_STATUS);
    intrLinkMask = DRF_VAL(_NVLW, _TOP_INTR_0_STATUS, _LINK, reg);

    //
    // Shift the mask of interrupting links from the local to the
    // NVLW instance to a global mask
    //
    intrLinkMask = intrLinkMask << (NVSWITCH_LINKS_PER_NVLW_LS10*instance);

    status[0] = device->hal.nvswitch_service_minion_link(device, instance);
    status[1] = _nvswitch_service_minion_fatal_ls10(device, instance);
    status[2] = _nvswitch_service_nvlipt_common_fatal_ls10(device, instance);

    //
    // If there is a pending link interrupt on this nvlw instance then service
    // those interrupts in the handlers below. Otherwise, mark the status's
    // as success as there is nothing to service
    //
    if (intrLinkMask != 0)
    {
        status[3] = _nvswitch_service_nvldl_fatal_ls10(device, instance, intrLinkMask);
        status[4] = _nvswitch_service_nvltlc_fatal_ls10(device, instance, intrLinkMask);
        status[5] = _nvswitch_service_nvlipt_link_fatal_ls10(device, instance, intrLinkMask);
    }
    else
    {
        status[3] = NVL_SUCCESS;
        status[4] = NVL_SUCCESS;
        status[5] = NVL_SUCCESS;
    }

    if (status[0] != NVL_SUCCESS && status[0] != -NVL_NOT_FOUND &&
        status[1] != NVL_SUCCESS && status[1] != -NVL_NOT_FOUND &&
        status[2] != NVL_SUCCESS && status[2] != -NVL_NOT_FOUND &&
        status[3] != NVL_SUCCESS && status[3] != -NVL_NOT_FOUND &&
        status[4] != NVL_SUCCESS && status[4] != -NVL_NOT_FOUND &&
        status[5] != NVL_SUCCESS && status[5] != -NVL_NOT_FOUND)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Enable top level HW interrupts.
 *
 * @Description :
 *
 * @param[in] device        operate on this device
 */
void
nvswitch_lib_enable_interrupts_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NPG_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NPG_CORRECTABLE_IDX), 0);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NVLW_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NVLW_CORRECTABLE_IDX), 0);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_NXBAR_FATAL_IDX), 0x7);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_UNITS_IDX), 0xFFFFFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_SET(NV_CTRL_CPU_INTR_UNITS_IDX),
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PMGR_HOST, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER_ALARM, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _SEC0_INTR0_0, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XTL_CPU, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XAL_EP, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PRIV_RING, 1));

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP_EN_SET(0), 0xFFFFFFFF);
}

/*
 * @Brief : Disable top level HW interrupts.
 *
 * @Description :
 *
 * @param[in] device        operate on this device
 */
void
nvswitch_lib_disable_interrupts_ls10
(
    nvswitch_device *device
)
{
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NPG_CORRECTABLE_IDX), 0);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NVLW_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX), 0xFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NVLW_CORRECTABLE_IDX), 0);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_NXBAR_FATAL_IDX), 0x7);

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF_EN_CLEAR(NV_CTRL_CPU_INTR_UNITS_IDX),
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PMGR_HOST, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER_ALARM, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _SEC0_INTR0_0, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XTL_CPU, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XAL_EP, 1) |
        DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PRIV_RING, 1));

    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP_EN_CLEAR(0), 0xFFFFFFFF);
}

//
// Check if there are interrupts pending.
//
// On silicon/emulation we only use MSIs which are not shared, so this
// function does not need to be called.
//
NvlStatus
nvswitch_lib_check_interrupts_ls10
(
    nvswitch_device *device
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU32 val;

    val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP(0));
    if (DRF_NUM(_CTRL, _CPU_INTR_TOP, _VALUE, val) != 0)
    {
        retval = -NVL_MORE_PROCESSING_REQUIRED;
    }

    return retval;
}

static void
_nvswitch_retrigger_engine_intr_ls10
(
    nvswitch_device *device
)
{

    // re-trigger engine to gin interrupts for CPR and NPG on the FATAL and NONFATAL trees
    NVSWITCH_BCAST_WR32_LS10(device, CPR, _CPR_SYS, _INTR_RETRIGGER(0), 1);
    NVSWITCH_BCAST_WR32_LS10(device, CPR, _CPR_SYS, _INTR_RETRIGGER(1), 1);

    NVSWITCH_BCAST_WR32_LS10(device, NPG, _NPG, _INTR_RETRIGGER(0), 1);
    NVSWITCH_BCAST_WR32_LS10(device, NPG, _NPG, _INTR_RETRIGGER(1), 1);
}

void
nvswitch_service_minion_all_links_ls10
(
    nvswitch_device *device
)
{
    NvU32 val, i;

    // Check NVLW
    val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NVLW_FATAL);
    val = DRF_NUM(_CTRL, _CPU_INTR_NVLW_FATAL, _MASK, val);
    if (val != 0)
    {
        NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL,
            _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_FATAL_IDX), val);

        for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NVLW_FATAL_MASK); i++)
        {
            if (val & NVBIT(i))
                (void)_nvswitch_service_nvlw_fatal_ls10(device, i);
        }
    }
}

//
// Service interrupt and re-enable interrupts. Interrupts should disabled when
// this is called.
//
NvlStatus
nvswitch_lib_service_interrupts_ls10
(
    nvswitch_device *device
)
{
    NvlStatus   status = NVL_SUCCESS;
    NvlStatus   return_status = NVL_SUCCESS;
    NvU32 val;
    NvU32 topEnable;
    NvU32 topIntr;
    NvU32 i;

    //
    // Interrupt handler steps:
    // 1. Read Top Interrupt
    // 1. Read Leaf interrupt
    // 2. Clear leaf interrupt
    // 3. Run leaf specific interrupt handler
    //

    topEnable = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP_EN_SET(0));
    topIntr   = topEnable & NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP(0));

    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NVLW_NON_FATAL, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NVLW_NON_FATAL);
        val = DRF_VAL(_CTRL, _CPU_INTR_NVLW_NON_FATAL, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, INFO, "%s: NVLW NON_FATAL interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_IDX), val);
            for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NVLW_NON_FATAL_MASK); i++)
            {
                if (val & NVBIT(i))
                {
                    status = _nvswitch_service_nvlw_nonfatal_ls10(device, i);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, INFO, "%s: NVLW[%d] NON_FATAL interrupt handling status = %d\n",
                            __FUNCTION__, i, status);
                        return_status = status;
                    }
                }
            }
        }
    }

    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NVLW_FATAL, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NVLW_FATAL);
        val = DRF_VAL(_CTRL, _CPU_INTR_NVLW_FATAL, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, INFO, "%s: NVLW FATAL interrupts pending = 0x%x\n",
                __FUNCTION__, val);

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NVLW_FATAL_IDX), val);

            for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NVLW_FATAL_MASK); i++)
            {
                if (val & NVBIT(i))
                {
                    status = _nvswitch_service_nvlw_fatal_ls10(device, i);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, INFO, "%s: NVLW[%d] FATAL interrupt handling status = %d\n",
                            __FUNCTION__, i, status);
                        return_status = status;
                    }
                }
            }
        }
    }

    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NVLW_CORRECTABLE, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NVLW_CORRECTABLE);
        val = DRF_VAL(_CTRL, _CPU_INTR_NVLW_CORRECTABLE, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: NVLW CORRECTABLE interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            return_status = -NVL_MORE_PROCESSING_REQUIRED;
        }
    }

    // Check NPG
    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NPG_FATAL, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NPG_FATAL);
        val = DRF_VAL(_CTRL, _CPU_INTR_NPG_FATAL, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, INFO, "%s: NPG FATAL interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NPG_FATAL_IDX), val);
            for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NPG_FATAL_MASK); i++)
            {
                if (val & NVBIT(i))
                {
                    status = _nvswitch_service_npg_fatal_ls10(device, i);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, INFO, "%s: NPG[%d] FATAL interrupt handling status = %d\n",
                            __FUNCTION__, i, status);
                        return_status = status;
                    }
                }
            }
        }
    }

    // Check NPG
    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NPG_NON_FATAL, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NPG_NON_FATAL);
        val = DRF_VAL(_CTRL, _CPU_INTR_NPG_NON_FATAL, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, INFO, "%s: NPG NON_FATAL interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NPG_NON_FATAL_IDX), val);
            for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NPG_NON_FATAL_MASK); i++)
            {
                if (val & NVBIT(i))
                {
                    status = _nvswitch_service_npg_nonfatal_ls10(device, i);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, INFO, "%s: NPG[%d] NON_FATAL interrupt handling status = %d\n",
                            __FUNCTION__, i, status);
                        return_status = status;
                    }
                }
            }
        }
    }

    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NPG_CORRECTABLE, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NPG_CORRECTABLE);
        val = DRF_VAL(_CTRL, _CPU_INTR_NPG_CORRECTABLE, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: NPG CORRECTABLE interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            return_status = -NVL_MORE_PROCESSING_REQUIRED;
        }
    }

    // Check NXBAR
    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_NXBAR_FATAL, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_NXBAR_FATAL);
        val = DRF_VAL(_CTRL, _CPU_INTR_NXBAR_FATAL, _MASK, val);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, INFO, "%s: NXBAR FATAL interrupts pending = 0x%x\n",
                __FUNCTION__, val);
            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_NXBAR_FATAL_IDX), val);
            for (i = 0; i < DRF_SIZE(NV_CTRL_CPU_INTR_NXBAR_FATAL_MASK); i++)
            {
                if (val & NVBIT(i))
                {
                    status = _nvswitch_service_nxbar_fatal_ls10(device, i);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, INFO, "%s: NXBAR[%d] FATAL interrupt handling status = %d\n",
                            __FUNCTION__, i, status);
                        return_status = status;
                    }
                }
            }
        }
    }

    // Check UNITS
    if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_TOP_LEAF, _INTR_UNITS, 0x1, topIntr))
    {
        val = NVSWITCH_ENG_RD32(device, GIN, , 0, _CTRL, _CPU_INTR_UNITS);
        if (val != 0)
        {
            NVSWITCH_PRINT(device, MMIO, "%s: UNIT interrupts pending = 0x%x\n",
                __FUNCTION__, val);

            NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_LEAF(NV_CTRL_CPU_INTR_UNITS_IDX), val);
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PMGR_HOST, 1, val))
            {
                NVSWITCH_PRINT(device, ERROR, "%s: _PMGR_HOST interrupt pending\n",
                    __FUNCTION__);
                return_status = -NVL_MORE_PROCESSING_REQUIRED;
            }
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER, 1, val))
            {
                NVSWITCH_PRINT(device, ERROR, "%s: _PTIMER interrupt pending\n",
                    __FUNCTION__);
                return_status = -NVL_MORE_PROCESSING_REQUIRED;
            }
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PTIMER_ALARM, 1, val))
            {
                NVSWITCH_PRINT(device, ERROR, "%s: _PTIMER_ALARM interrupt pending\n",
                    __FUNCTION__);
                return_status = -NVL_MORE_PROCESSING_REQUIRED;
            }
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XTL_CPU, 1, val))
            {
                NVSWITCH_PRINT(device, ERROR, "%s: _XTL_CPU interrupt pending\n",
                    __FUNCTION__);
                return_status = -NVL_MORE_PROCESSING_REQUIRED;
            }
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _XAL_EP, 1, val))
            {
                NVSWITCH_PRINT(device, ERROR, "%s: _XAL_EP interrupt pending\n",
                    __FUNCTION__);
                return_status = -NVL_MORE_PROCESSING_REQUIRED;
            }
            if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _PRIV_RING, 1, val))
            {
                status = _nvswitch_service_priv_ring_ls10(device);
                if (status != NVL_SUCCESS)
                {
                    NVSWITCH_PRINT(device, ERROR, "%s: Problem handling PRI errors\n",
                        __FUNCTION__);
                    return_status = status;
                }
            }
            if (!IS_RTLSIM(device) && !IS_FMODEL(device))
            {
                if (FLD_TEST_DRF_NUM(_CTRL, _CPU_INTR_UNITS, _SEC0_INTR0_0, 1, val))
                {
                    status = _nvswitch_service_soe_fatal_ls10(device);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, ERROR, "%s: Problem servicing SOE",
                            __FUNCTION__);
                        return_status = status;
                    }
                }
            }
        }
    }

    // step 4 -- retrigger engine interrupts
    _nvswitch_retrigger_engine_intr_ls10(device);

    // step 5 -- retrigger top level GIN interrupts
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP_EN_CLEAR(0), 0xFFFFFFFF);
    NVSWITCH_ENG_WR32(device, GIN, , 0, _CTRL, _CPU_INTR_TOP_EN_SET(0), 0xFFFFFFFF);

    return return_status;
}

/*
 * Initialize interrupt tree HW for all units.
 *
 * Init and servicing both depend on bits matching across STATUS/MASK
 * and IErr STATUS/LOG/REPORT/CONTAIN registers.
 */
void
nvswitch_initialize_interrupt_tree_ls10
(
    nvswitch_device *device
)
{
    NvU64 link_mask = nvswitch_get_enabled_link_mask(device);
    NvU32 i, val;

    // NPG/NPORT
    _nvswitch_initialize_nport_interrupts_ls10(device);

    // NXBAR
    _nvswitch_initialize_nxbar_interrupts_ls10(device);

    FOR_EACH_INDEX_IN_MASK(64, i, link_mask)
    {
        val = NVSWITCH_LINK_RD32(device, i,
                  NVLW, _NVLW, _LINK_INTR_0_MASK(i));
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_0_MASK, _FATAL,       _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_0_MASK, _NONFATAL,    _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_0_MASK, _CORRECTABLE, _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_0_MASK, _INTR0,       _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_0_MASK, _INTR1,       _ENABLE, val);
        NVSWITCH_LINK_WR32(device, i, NVLW, _NVLW, _LINK_INTR_0_MASK(i), val);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    FOR_EACH_INDEX_IN_MASK(64, i, link_mask)
    {
        val = NVSWITCH_LINK_RD32(device, i,
                  NVLW, _NVLW, _LINK_INTR_1_MASK(i));
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_1_MASK, _FATAL,       _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_1_MASK, _NONFATAL,    _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_1_MASK, _CORRECTABLE, _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_1_MASK, _INTR0,       _ENABLE, val);
        val = FLD_SET_DRF(_NVLW, _LINK_INTR_1_MASK, _INTR1,       _ENABLE, val);
        NVSWITCH_LINK_WR32(device, i, NVLW, _NVLW, _LINK_INTR_1_MASK(i), val);
    }
    FOR_EACH_INDEX_IN_MASK_END;

   // NVLIPT
    _nvswitch_initialize_nvlipt_interrupts_ls10(device);
}

//
// Service Nvswitch NVLDL Fatal interrupts
//
NvlStatus
nvswitch_service_nvldl_fatal_link_ls10
(
    nvswitch_device *device,
    NvU32 nvlipt_instance,
    NvU32 link
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);
    NvU32 pending, bit, unhandled;
    NvU32 dlDeferredIntrLinkMask = 0;
    NvBool bRequireResetAndDrain = NV_FALSE;

    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    INFOROM_NVLINK_ERROR_EVENT error_event;

    report.raw_pending = NVSWITCH_LINK_RD32(device, link, NVLDL, _NVLDL_TOP, _INTR);
    report.raw_enable = NVSWITCH_LINK_RD32(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN);
    report.mask = report.raw_enable;
    pending = report.raw_pending & report.mask;

    error_event.nvliptInstance = (NvU8) nvlipt_instance;
    error_event.localLinkIdx   = (NvU8) NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LS10(link);

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

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_PROTOCOL, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_LTSSM_PROTOCOL, "LTSSM Protocol Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);

        // TODO 2827793 this should be logged to the InfoROM as fatal
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _PHY_A, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_PHY_A, "PHY_A Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_PHY_A_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _TX_PL_ERROR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_TX_PL_ERROR, "TX_PL Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_TX_PL_ERROR_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
   }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _RX_PL_ERROR, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        NVSWITCH_REPORT_FATAL(_HW_DLPL_RX_PL_ERROR, "RX_PL Error", NV_FALSE);
        nvswitch_clear_flags(&unhandled, bit);
        error_event.error = INFOROM_NVLINK_DL_RX_PL_ERROR_FATAL;
        nvswitch_inforom_nvlink_log_error_event(device, &error_event);
    }

    //
    // Note: LTSSM_FAULT_{UP/DOWN} must be the last interrupt serviced in the NVLDL
    // Fatal tree. The last step of handling this interrupt is going into the
    // reset_and_drain flow for the given link which will shutdown and reset
    // the link. The reset portion will also wipe away any link state including
    // pending DL interrupts. In order to log all error before wiping that state,
    // service all other interrupts before this one
    //
    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_DOWN, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        nvswitch_record_port_event(device, &(device->log_PORT_EVENTS), link, NVSWITCH_PORT_EVENT_TYPE_DOWN);
        if (nvswitch_lib_notify_client_events(device,
                    NVSWITCH_DEVICE_EVENT_PORT_DOWN) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify PORT_DOWN event\n",
                         __FUNCTION__);
        }
        dlDeferredIntrLinkMask |= bit;

        //
        // Disable LTSSM FAULT DOWN, NPG, and NVLW interrupts to avoid interrupt storm. The interrupts 
        //  will be re-enabled in reset and drain
        //
        report.raw_enable = FLD_SET_DRF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_FAULT_DOWN, _DISABLE, report.raw_enable);
        NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN, report.raw_enable);
        nvswitch_link_disable_interrupts_ls10(device, link);

        if (device->bModeContinuousALI)
        {
            //
            // Since reset and drain will reset the link, including clearing
            // pending interrupts, skip the clear write below. There are cases
            // where link clocks will not be on after reset and drain so there
            // maybe PRI errors on writing to the register
            //
            // CCI will perform reset and drain
            if (!cciIsLinkManaged(device, link))
            {
                bRequireResetAndDrain = NV_TRUE;
            }
        }
        nvswitch_clear_flags(&unhandled, bit);
    }

    bit = DRF_NUM(_NVLDL_TOP, _INTR, _LTSSM_FAULT_UP, 1);
    if (nvswitch_test_flags(pending, bit))
    {
        nvswitch_record_port_event(device, &(device->log_PORT_EVENTS), link, NVSWITCH_PORT_EVENT_TYPE_DOWN);
        if (nvswitch_lib_notify_client_events(device,
                    NVSWITCH_DEVICE_EVENT_PORT_DOWN) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify PORT_DOWN event\n",
                         __FUNCTION__);
        }
        dlDeferredIntrLinkMask |= bit;

        //
        // Disable LTSSM FAULT UP, NPG, and NVLW link interrupts to avoid interrupt storm. The interrupts
        //  will be re-enabled in reset and drain
        //
        report.raw_enable = FLD_SET_DRF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_FAULT_UP, _DISABLE, report.raw_enable);
        NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN, report.raw_enable);
        nvswitch_link_disable_interrupts_ls10(device, link);
        
        if (device->bModeContinuousALI)
        {
            //
            // Since reset and drain will reset the link, including clearing
            // pending interrupts, skip the clear write below. There are cases
            // where link clocks will not be on after reset and drain so there
            // maybe PRI errors on writing to the register
            //
            // CCI will perform reset and drain
            if (!cciIsLinkManaged(device, link))
            {
                bRequireResetAndDrain = NV_TRUE;
            }
        }
        nvswitch_clear_flags(&unhandled, bit);
    }

    if (bRequireResetAndDrain)
    {
        device->hal.nvswitch_reset_and_drain_links(device, NVBIT64(link), NV_FALSE);
    }

    chip_device->deferredLinkErrors[link].data.fatalIntrMask.dl |= dlDeferredIntrLinkMask;
    if (dlDeferredIntrLinkMask)
    {
        nvswitch_create_deferred_link_state_check_task_ls10(device, nvlipt_instance, link);
    }

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    // Disable interrupts that have occurred after fatal error.
    if (device->link[link].fatal_error_occurred)
    {
        NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR_STALL_EN,
                report.raw_enable & ~pending);
    }

    if (!bRequireResetAndDrain)
    {
        NVSWITCH_LINK_WR32(device, link, NVLDL, _NVLDL_TOP, _INTR, pending);
    }

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
nvswitch_service_minion_link_ls10
(
    nvswitch_device *device,
    NvU32 instance
)
{
    NVSWITCH_INTERRUPT_LOG_TYPE report = { 0 };
    NvU32 pending, unhandled, minionIntr, linkIntr, reg, enabledLinks, bit;
    NvU32 localLinkIdx, link;
    MINION_LINK_INTR minionLinkIntr = { 0 };
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    //
    // _MINION_MINION_INTR shows all interrupts currently at the host on this minion
    // Note: _MINIO_MINION_INTR is not used to clear link specific interrupts
    //
    minionIntr = NVSWITCH_MINION_RD32_LS10(device, instance, _MINION, _MINION_INTR);

    // get all possible interrupting links associated with this minion
    report.raw_pending = DRF_VAL(_MINION, _MINION_INTR, _LINK, minionIntr);

    // read in the enaled minion interrupts on this minion
    reg = NVSWITCH_MINION_RD32_LS10(device, instance, _MINION, _MINION_INTR_STALL_EN);

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

    minionLinkIntr.bPending = NV_TRUE;

    FOR_EACH_INDEX_IN_MASK(32, localLinkIdx, pending)
    {
        link = (instance * NVSWITCH_LINKS_PER_NVLIPT_LS10) + localLinkIdx;
        bit = NVBIT(localLinkIdx);

        // read in the interrupt register for the given link
        linkIntr = NVSWITCH_MINION_LINK_RD32_LS10(device, link, _MINION, _NVLINK_LINK_INTR(localLinkIdx));
        minionLinkIntr.regData = linkIntr;

        // _STATE must be set for _CODE to be valid
        if (!DRF_VAL(_MINION, _NVLINK_LINK_INTR, _STATE, linkIntr))
        {
            continue;
        }

        NVSWITCH_PRINT(device, INFO,
                "%s: link[%d] {%d, %d} linkIntr = 0x%x\n",
                 __FUNCTION__, link, instance, localLinkIdx, linkIntr);

        //
        // _MINION_INTR_LINK is a read-only register field for the host
        // Host must write 1 to _NVLINK_LINK_INTR_STATE to clear the interrupt on the link
        //
        reg = DRF_NUM(_MINION, _NVLINK_LINK_INTR, _STATE, 1);
        NVSWITCH_MINION_WR32_LS10(device, instance, _MINION, _NVLINK_LINK_INTR(localLinkIdx), reg);

        report.data[0] = linkIntr;

        switch(DRF_VAL(_MINION, _NVLINK_LINK_INTR, _CODE, linkIntr))
        {
            case NV_MINION_NVLINK_LINK_INTR_CODE_NA:
            case NV_MINION_NVLINK_LINK_INTR_CODE_DLCMDFAULT:
            case NV_MINION_NVLINK_LINK_INTR_CODE_LOCAL_CONFIG_ERR:
            case NV_MINION_NVLINK_LINK_INTR_CODE_NEGOTIATION_CONFIG_ERR:
            case NV_MINION_NVLINK_LINK_INTR_CODE_BADINIT:
            case NV_MINION_NVLINK_LINK_INTR_CODE_PMFAIL:
            case NV_MINION_NVLINK_LINK_INTR_CODE_NOINIT:
                chip_device->deferredLinkErrors[link].data.fatalIntrMask.minionLinkIntr =
                    minionLinkIntr;
                _nvswitch_create_deferred_link_errors_task_ls10(device, instance, link);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_SWREQ:
                NVSWITCH_PRINT(device, INFO,
                      "%s: Received MINION Link SW Generate interrupt on MINION %d : link %d.\n",
                      __FUNCTION__, instance, link);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_DLREQ:
            case NV_MINION_NVLINK_LINK_INTR_CODE_PMDISABLED:
            case NV_MINION_NVLINK_LINK_INTR_CODE_TLREQ:
                chip_device->deferredLinkErrors[link].data.nonFatalIntrMask.minionLinkIntr =
                    minionLinkIntr;
                _nvswitch_create_deferred_link_errors_task_ls10(device, instance, link);
            case NV_MINION_NVLINK_LINK_INTR_CODE_NOTIFY:
                NVSWITCH_PRINT(device, INFO,
                      "%s: Received MINION NOTIFY interrupt on MINION %d : link %d.\n",
                      __FUNCTION__, instance, link);
                break;
            case NV_MINION_NVLINK_LINK_INTR_CODE_INBAND_BUFFER_AVAILABLE:
            {
                NVSWITCH_PRINT(device, INFO,
                      "Received INBAND_BUFFER_AVAILABLE interrupt on MINION %d,\n", instance);
                nvswitch_minion_receive_inband_data_ls10(device, link);
                break;
            }
            default:
                NVSWITCH_REPORT_FATAL(_HW_MINION_FATAL_LINK_INTR, "Minion Interrupt code unknown", NV_FALSE);
        }
        nvswitch_clear_flags(&unhandled, bit);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    NVSWITCH_UNHANDLED_CHECK(device, unhandled);

    if (unhandled != 0)
    {
        return -NVL_MORE_PROCESSING_REQUIRED;
    }

    return NVL_SUCCESS;
}
