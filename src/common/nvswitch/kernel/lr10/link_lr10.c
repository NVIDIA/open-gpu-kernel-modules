/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink_export.h"

#include "export_nvswitch.h"
#include "common_nvswitch.h"
#include "regkey_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/minion_lr10.h"
#include "lr10/pmgr_lr10.h"

#include "nvswitch/lr10/dev_nvldl_ip.h"
#include "nvswitch/lr10/dev_nvldl_ip_addendum.h"
#include "nvswitch/lr10/dev_minion_ip_addendum.h"
#include "nvswitch/lr10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/lr10/dev_nvlphyctl_ip.h"
#include "nvswitch/lr10/dev_nvltlc_ip.h"
#include "nvswitch/lr10/dev_minion_ip.h"
#include "nvswitch/lr10/dev_trim.h"
#include "nvswitch/lr10/dev_pri_ringstation_sys.h"
#include "nvswitch/lr10/dev_nvlperf_ip.h"
#include "nvswitch/lr10/dev_nvlipt_ip.h"
#include "nvswitch/lr10/dev_nport_ip.h"

#define NUM_SWITCH_WITH_DISCONNETED_REMOTE_LINK 12 // This must be incremented if any entries are added to the array below
lr10_links_connected_to_disabled_remote_end nvswitchDisconnetedRemoteLinkMasks[] =
{
    {
        0x8,         // switchPhysicalId
        0x56A000500, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x9,         // switchPhysicalId
        0x509009900, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0xa,         // switchPhysicalId
        0x0,         // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0xb,         // switchPhysicalId
        0x56A000600, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0xc,         // switchPhysicalId
        0x4A9009400, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0xd,         // switchPhysicalId
        0x0,         // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x18,        // switchPhysicalId
        0x56A000500, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x19,        // switchPhysicalId
        0x509009900, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x1a,        // switchPhysicalId
        0x0,         // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x1b,        // switchPhysicalId
        0x56A000600, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x1c,        // switchPhysicalId
        0x4A9009400, // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
    {
        0x1d,        // switchPhysicalId
        0x0,         // accessLinkMask
        0xFF00FF     // trunkLinkMask
    },
};
ct_assert(sizeof(nvswitchDisconnetedRemoteLinkMasks)/sizeof(lr10_links_connected_to_disabled_remote_end) == NUM_SWITCH_WITH_DISCONNETED_REMOTE_LINK);


void
nvswitch_setup_link_loopback_mode_lr10
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    nvlink_link *link;
    NV_STATUS status;

    link = nvswitch_get_link(device, linkNumber);

    if ((link == NULL) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
        (linkNumber >= NVSWITCH_NVLINK_MAX_LINKS))
    {
        return;
    }

    if (device->link[link->linkNumber].nea)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Setting NEA on link %d\n",
            __FUNCTION__, link->linkNumber);

        status = nvswitch_minion_send_command(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_SETNEA, 0);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SETNEA CMD failed for link %d.\n",
                __FUNCTION__, link->linkNumber);
        }
    }

    if (device->link[link->linkNumber].nedr)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Setting NEDR on link %d\n",
            __FUNCTION__, link->linkNumber);

        // setting NEDR
        status = nvswitch_minion_send_command(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_SETNEDR, 0);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SETNEDR CMD failed for link %d.\n",
                __FUNCTION__, link->linkNumber);
        }
    }
        
    if (device->link[link->linkNumber].nedw)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Setting NEDW on link %d\n",
            __FUNCTION__, link->linkNumber);

        // setting NEDW
        status = nvswitch_minion_send_command(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_SETNEDW, 0);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SETNEDW CMD failed for link %d.\n",
                __FUNCTION__, link->linkNumber);
        }
    }
}

static NV_STATUS
_nvswitch_ioctrl_setup_link_plls_lr10
(
    nvlink_link *link
)
{
    NV_STATUS status = NV_OK;
    NvU32     linkId, tempRegVal;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    nvswitch_device *device = link->dev->pDevInfo;
    linkId = link->linkNumber;

    if (IS_EMULATION(device))
    {
        NVSWITCH_PRINT(device, ERROR,"Skipping PLL init on emulation. \n");
        return status;
    }

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * 400, &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;
        tempRegVal = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLIPT_LNK , _NVLIPT_LNK , _CTRL_CLK_CTRL);

        if (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _PLL_PWR_STS, _ON, tempRegVal))
            break;

        nvswitch_os_sleep(1);
    } while (keepPolling == NV_TRUE);

    if (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _PLL_PWR_STS, _OFF, tempRegVal))
    {
        NVSWITCH_PRINT(device, ERROR,
                  "PLL_PWR_STS did not turn _ON for linkId = 0x%x!!\n", linkId);
        return NV_ERR_TIMEOUT;
    }

    // Request Minion to setup the NVLink clocks
    status = nvswitch_minion_send_command(device, linkId,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_TXCLKSWITCH_PLL, 0);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
                  "Error sending TXCLKSWITCH_PLL command to MINION. Link = %d\n", linkId);
        NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CLOCK_ERROR, NVBIT32(link->linkNumber), TXCLKSWITCH_PLL_ERROR);
        return NV_ERR_NVLINK_CLOCK_ERROR;
    }

    // Poll for the links to switch to NVLink clocks
    nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * 400, &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;
        tempRegVal = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLIPT_LNK , _NVLIPT_LNK , _CTRL_CLK_CTRL);
        if (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _TXCLK_STS, _PLL_CLK, tempRegVal))
            break;

        nvswitch_os_sleep(1);
    } while (keepPolling == NV_TRUE);

    if (!FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _TXCLK_STS, _PLL_CLK, tempRegVal))
    {
        // Print the links for which we were unable to switch to PLL clock
        NVSWITCH_PRINT(device, ERROR,
                  "TXCLK_STS did not switch to _PLL_CLOCK for linkId = 0x%x!!\n", linkId);
        return NV_ERR_TIMEOUT;
    }

    return status;
}

NvBool
nvswitch_is_link_in_reset_lr10
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    NvU32 val;
    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET);

    return (FLD_TEST_DRF(_NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET, _LINK_RESET_STATUS,
                _ASSERTED, val)) ? NV_TRUE : NV_FALSE;
}

NvlStatus
nvswitch_poll_sublink_state_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32 val;
    NvBool bPreSiPlatform = (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device));

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * (bPreSiPlatform ? 2000: 200), &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE);

        if (FLD_TEST_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _STATUS, _FAULT, val))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s : Fault while changing sublink state (%s):(%s).\n",
                __FUNCTION__, device->name, link->linkName);
            return -NVL_ERR_INVALID_STATE;
        }

        if (FLD_TEST_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _STATUS, _DONE, val))
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if ((!FLD_TEST_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _STATUS, _DONE, val)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : Timeout while waiting sublink state (%s):(%s).\n",
            __FUNCTION__, device->name, link->linkName);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_init_dl_pll
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvlStatus status;

    status = nvswitch_minion_send_command(device, link->linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_INITPLL, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: INITPLL failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CLOCK_ERROR, NVBIT32(link->linkNumber), INITPLL_ERROR);
        return NV_ERR_NVLINK_CLOCK_ERROR;
    }

    status = _nvswitch_ioctrl_setup_link_plls_lr10(link);
    if (status != NV_OK){
        return status;
    }

    status = nvswitch_minion_send_command(device, link->linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHY, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: INITPHY failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_INIT_ERROR, NVBIT32(link->linkNumber), INITPHY_ERROR);
        return NV_ERR_NVLINK_INIT_ERROR;
    }


    return NVL_SUCCESS;
}

NvU32
nvswitch_get_sublink_width_lr10
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    return NVSWITCH_NUM_LANES_LR10;
}

void
nvswitch_init_dlpl_interrupts_lr10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNumber = link->linkNumber;
    NvU32            crcShortRegkeyVal = device->regkeys.crc_bit_error_rate_short;
    NvU32            crcLongRegkeyVal  = device->regkeys.crc_bit_error_rate_long;
    NvU32            intrRegVal;
    NvU32            crcRegVal;
    NvU32            shortRateMask;
    NvU32            longRateMask;

    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN)   == 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_THRESHOLD_MAN));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN) == 
              DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_THRESHOLD_MAN));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP)   == 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_THRESHOLD_EXP));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP) == 
              DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_THRESHOLD_EXP));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN)   == 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_TIMESCALE_MAN));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN) == 
              DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_TIMESCALE_MAN));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP)   == 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_TIMESCALE_EXP));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP) == 
              DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_SHORT_TIMESCALE_EXP));
    
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_MAN)    == 
             (DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)   - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_MAN)  == 
             (DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN) - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_EXP)    == 
             (DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_EXP)   - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_EXP)  == 
             (DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_EXP) - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_MAN)    == 
             (DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_TIMESCALE_MAN)   - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_MAN)  == 
             (DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_TIMESCALE_MAN) - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_BASE(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_EXP)    == 
             (DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_TIMESCALE_EXP)   - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));
    ct_assert(DRF_EXTENT(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_EXP)  == 
             (DRF_EXTENT(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_TIMESCALE_EXP) - 
              DRF_BASE(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN)));

    // W1C any stale state.
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR, 0xffffffff);
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_SW2, 0xffffffff);

    // Stall tree routes to INTR_A which is connected to NVLIPT fatal tree
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_STALL_EN,
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_REPLAY, _DISABLE)               |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_RECOVERY_SHORT, _DISABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_FAULT_UP, _ENABLE)           |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_RAM, _ENABLE)             |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_INTERFACE, _ENABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_FAULT_DL_PROTOCOL, _ENABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_SHORT_ERROR_RATE, _DISABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_LONG_ERROR_RATE, _DISABLE)      |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_ILA_TRIGGER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_CRC_COUNTER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_PROTOCOL, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _MINION_REQUEST, _DISABLE));

    // NONSTALL -> NONFATAL
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN,
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_REPLAY, _DISABLE)               |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_RECOVERY_SHORT, _DISABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _LTSSM_FAULT_UP, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_RAM, _DISABLE)            |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_INTERFACE, _DISABLE)      |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_FAULT_DL_PROTOCOL, _DISABLE)    |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, _DISABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_LONG_ERROR_RATE, _DISABLE)      |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_ILA_TRIGGER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_CRC_COUNTER, _ENABLE)           |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _LTSSM_PROTOCOL, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _MINION_REQUEST, _DISABLE));

    intrRegVal = NVSWITCH_LINK_RD32_LR10(device, linkNumber, NVLDL, 
                                         _NVLDL_TOP, _INTR_NONSTALL_EN);
    crcRegVal  = NVSWITCH_LINK_RD32_LR10(device, linkNumber, NVLDL, 
                                         _NVLDL_RX, _ERROR_RATE_CTRL);

    // Enable RX error rate short interrupt if the regkey is set
    if (crcShortRegkeyVal != NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_DEFAULT)
    {
        shortRateMask = DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN)     |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP);
        
        intrRegVal |= DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, _ENABLE);
        crcRegVal  &= ~shortRateMask;
        crcRegVal  |= crcShortRegkeyVal;
    }
    // Enable RX error rate long interrupt if the regkey is set
    if (crcLongRegkeyVal != NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_DEFAULT)
    {
        longRateMask = DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_MAN)      |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_THRESHOLD_EXP) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_MAN) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_TIMESCALE_EXP);

        intrRegVal |= DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_LONG_ERROR_RATE, _ENABLE);
        crcRegVal  &= ~longRateMask;
        crcRegVal  |= crcLongRegkeyVal << DRF_SHIFT(NV_NVLDL_RX_ERROR_RATE_CTRL_LONG_THRESHOLD_MAN);
    }

    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, 
                            _NVLDL_TOP, _INTR_NONSTALL_EN, intrRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, 
                            _NVLDL_RX, _ERROR_RATE_CTRL, crcRegVal);
}

static void
_nvswitch_disable_dlpl_interrupts
(
    nvlink_link *link
)
{
    nvswitch_device *device    = link->dev->pDevInfo;
    NvU32           linkNumber = link->linkNumber;

    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_STALL_EN,    0x0);
    NVSWITCH_LINK_WR32_LR10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN, 0x0);
}

void
nvswitch_store_topology_information_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvU32            tempval;

    link->bInitnegotiateConfigGood = NV_TRUE;
    link->remoteSid = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_SID_HI);
    link->remoteSid = link->remoteSid << 32;
    link->remoteSid |= NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_SID_LO);

    tempval = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _TOPOLOGY_REMOTE_LINK_INFO);
    link->remoteLinkId = DRF_VAL(_NVLIPT_LNK, _TOPOLOGY_REMOTE_LINK_INFO, _LINK_NUMBER, tempval);

    link->localSid = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT,
                                            _NVLIPT_COMMON, _TOPOLOGY_LOCAL_CHIP_SID_HI);
    link->localSid = link->localSid << 32;
    link->localSid |= NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT,
                                             _NVLIPT_COMMON, _TOPOLOGY_LOCAL_CHIP_SID_LO);

    tempval = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_TYPE);

    // Update the remoteDeviceType with NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE values.
    switch(tempval)
    {
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV3P0AMP:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;
        break;
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV3P0LRK:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH;
        break;
        default:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE;
        break;
    }
}

void
nvswitch_init_lpwr_regs_lr10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNum = link->linkNumber;
    NvU32 tempRegVal, icLimit, fbIcInc, lpIcInc, fbIcDec, lpIcDec, lpEntryThreshold;
    NvU32 lpExitThreshold;
    NvU8  softwareDesired, hardwareDisable;
    NvBool bLpEnable;

    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        return;
    }

    if (nvswitch_is_link_in_reset(device, link))
    {
        return;
    }

    if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_NO)
    {
        return;
    }

    //
    // Power Management threshold settings
    // These settings are currently being hard coded.
    // They will be parsed from the VBIOS NVLink LPWR table once bug 2767390 is
    // implemented
    //

    // IC Limit
    icLimit = 16110000;

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_LIMIT, _LIMIT, icLimit,
        tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_LIMIT,
        tempRegVal);

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK,_PWRM_IC_LIMIT, _LIMIT, icLimit,
        tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_LIMIT,
        tempRegVal);

    //IC Inc
    fbIcInc = 1;
    lpIcInc = 1;

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_INC, _FBINC, fbIcInc, tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_INC, _LPINC, lpIcInc, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_INC,
        tempRegVal);

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_INC, _FBINC, fbIcInc, tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_INC, _LPINC, lpIcInc, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_INC,
        tempRegVal);

    //IC Dec
    fbIcDec = 1;
    lpIcDec = 65535;

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_DEC, _FBDEC, fbIcDec, tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_DEC, _LPDEC, lpIcDec, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_DEC,
        tempRegVal);

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_DEC, _FBDEC, fbIcDec,   tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_DEC, _LPDEC, lpIcDec, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_DEC,
        tempRegVal);

    //IC Enter Threshold
    if (device->regkeys.lp_threshold == NV_SWITCH_REGKEY_SET_LP_THRESHOLD_DEFAULT)
    {
        // TODO: get from bios. Refer Bug 3626523 for more info.
        lpEntryThreshold = 16110000;
    }
    else
    {
        lpEntryThreshold = device->regkeys.lp_threshold;
    }

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_LP_ENTER_THRESHOLD, _THRESHOLD, lpEntryThreshold, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_LP_ENTER_THRESHOLD,
        tempRegVal);

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_LP_ENTER_THRESHOLD, _THRESHOLD, lpEntryThreshold, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_LP_ENTER_THRESHOLD,
        tempRegVal);

    //IC Exit Threshold
    lpExitThreshold = 16044465;

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_LP_EXIT_THRESHOLD, _THRESHOLD, lpExitThreshold, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_LP_EXIT_THRESHOLD,
        tempRegVal);

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_LP_EXIT_THRESHOLD, _THRESHOLD, lpExitThreshold, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_LP_EXIT_THRESHOLD,
        tempRegVal);

    //LP Entry Enable
    bLpEnable = NV_TRUE;
    softwareDesired = (bLpEnable) ? 0x1 : 0x0;
    hardwareDisable = (bLpEnable) ? 0x0 : 0x1;

    tempRegVal = NVSWITCH_LINK_RD32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_SW_CTRL);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_SW_CTRL, _SOFTWAREDESIRED,
                    softwareDesired, tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _PWRM_IC_SW_CTRL, _HARDWAREDISABLE,
                    hardwareDisable, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _PWRM_IC_SW_CTRL,
                    tempRegVal);

    tempRegVal = NVSWITCH_LINK_RD32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_SW_CTRL);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_SW_CTRL, _SOFTWAREDESIRED,
                    softwareDesired, tempRegVal);
    tempRegVal = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _PWRM_IC_SW_CTRL, _HARDWAREDISABLE,
                    hardwareDisable, tempRegVal);
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _PWRM_IC_SW_CTRL,
                    tempRegVal);
}

void
nvswitch_program_l1_scratch_reg_lr10
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    // Not Implemented for LR10
}

void
nvswitch_init_buffer_ready_lr10
(
    nvswitch_device *device,
    nvlink_link *link,
    NvBool bNportBufferReady
)
{
    NvU32 val;
    NvU32 linkNum = link->linkNumber;

    if (FLD_TEST_DRF(_SWITCH_REGKEY, _SKIP_BUFFER_READY, _TLC, _NO,
                     device->regkeys.skip_buffer_ready))
    {
        val = DRF_NUM(_NVLTLC_RX_SYS, _CTRL_BUFFER_READY, _BUFFERRDY, 0x1);
        NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_SYS, _CTRL_BUFFER_READY, val);
        val = DRF_NUM(_NVLTLC_TX_SYS, _CTRL_BUFFER_READY, _BUFFERRDY, 0x1);
        NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_SYS, _CTRL_BUFFER_READY, val);
    }

    if (bNportBufferReady &&
        FLD_TEST_DRF(_SWITCH_REGKEY, _SKIP_BUFFER_READY, _NPORT, _NO,
                     device->regkeys.skip_buffer_ready))
    {
        val = DRF_NUM(_NPORT, _CTRL_BUFFER_READY, _BUFFERRDY, 0x1);
        NVSWITCH_LINK_WR32_LR10(device, linkNum, NPORT, _NPORT, _CTRL_BUFFER_READY, val);
    }
}

static void
_nvswitch_configure_reserved_throughput_counters
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNum = link->linkNumber;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLTLC, link->linkNumber))
    {
        NVSWITCH_PRINT(device, INFO,
                       "Invalid link, skipping NVLink throughput counter config for link %d\n",
                       link->linkNumber);
        return;
    }

    //
    // Counters 0 and 2 will be reserved for monitoring tools
    // Counters 1 and 3 will be user-configurable and used by devtools
    //

    // Rx0 config
    NVSWITCH_LINK_WR32_IDX_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, 0,
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)      |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT));

    // Tx0 config
    NVSWITCH_LINK_WR32_IDX_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, 0,
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)      |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT));

    // Rx2 config
    NVSWITCH_LINK_WR32_IDX_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, 2,
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)      |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _HEAD) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _AE) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _BE) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT));

    // Tx2 config
    NVSWITCH_LINK_WR32_IDX_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, 2,
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)      |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _HEAD) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _AE) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _BE) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT));

    // Enable Rx for counters 0, 2
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL,
        DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL, _ENRX0, 0x1) |
        DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL, _ENRX2, 0x1));

    // Enable Tx for counters 0, 2
    NVSWITCH_LINK_WR32_LR10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL,
        DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL, _ENTX0, 0x1) |
        DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL, _ENTX2, 0x1));
}

static NvlStatus
_nvswitch_init_link_post_active
(
    nvlink_link *link,
    NvU32       flags
)
{
    NvlStatus       status = NVL_SUCCESS;
    nvswitch_device *device = link->dev->pDevInfo;

    nvswitch_init_lpwr_regs(link);
    status = nvswitch_request_tl_link_state_lr10(link,
                NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_ACTIVE,
                flags == NVLINK_STATE_CHANGE_SYNC);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    // Note: buffer_rdy should be asserted last!
    nvswitch_init_buffer_ready(device, link, NV_TRUE);

    return status;
}

static void
_nvswitch_power_down_link_plls
(
    nvlink_link *link
)
{
    NvlStatus       status = NVL_SUCCESS;
    nvswitch_device *device = link->dev->pDevInfo;

    if (IS_EMULATION(device))
    {
        NVSWITCH_PRINT(device, ERROR,"Skipping PLL init on emulation. \n");
        return;
    }

    status = nvswitch_minion_send_command(device, link->linkNumber,
        NV_MINION_NVLINK_DL_CMD_COMMAND_TXCLKSWITCH_ALT, 0);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: TXCLKSWITCH_ALT CMD failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        return;
    }

    return;
}

NvlStatus
nvswitch_corelib_add_link_lr10
(
    nvlink_link *link
)
{
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_remove_link_lr10
(
    nvlink_link *link
)
{
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_set_dl_link_mode_lr10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32           val;
    NvU32           link_state;
    NvlStatus       status = NVL_SUCCESS;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    if (nvswitch_does_link_need_termination_enabled(device, link))
    {
        if (mode == NVLINK_LINKSTATE_INITPHASE1)
        {
            status = nvswitch_link_termination_setup(device, link);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to enable termination on link #%d\n", __FUNCTION__, link->linkNumber);
            }
        }

        // return SUCCESS to avoid errors being propogated
        return NVL_SUCCESS;
    }

    switch (mode)
    {
        case NVLINK_LINKSTATE_SAFE:
        {
            // check if link is in reset
            if (nvswitch_is_link_in_reset(device, link))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: link #%d is still in reset, cannot change link state\n",
                    __FUNCTION__, link->linkNumber);
                return NVL_ERR_INVALID_STATE;
            }

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_STATE);
            link_state = DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, val);

            if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_SWCFG)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : Link is already in Safe mode for (%s).\n",
                    __FUNCTION__, link->linkName);
                break;
            }
            else if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_HWCFG)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : Link already transitioning to Safe mode for (%s).\n",
                    __FUNCTION__, link->linkName);
                break;
            }

            NVSWITCH_PRINT(device, INFO,
                "NVRM: %s : Changing Link state to Safe for (%s):(%s).\n",
                __FUNCTION__, device->name, link->linkName);

            if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_INIT)
            {
                val = 0;
                val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _NEWSTATE, _HWCFG, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _OLDSTATE_MASK, _DONTCARE, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _ACTION, _LTSSM_CHANGE, val);
                NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE, val);
            }
            else if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_ACTIVE)
            {
                val = 0;
                val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _NEWSTATE, _SWCFG, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _OLDSTATE_MASK, _DONTCARE, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _ACTION, _LTSSM_CHANGE, val);
                NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE, val);
            }
            else
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Link is in invalid state"
                    " cannot set to safe state (%s):(%s). (%x) (%x)\n",
                    __FUNCTION__, device->name, link->linkName, val, link_state);
                return -NVL_ERR_INVALID_STATE;
            }

            break;
        }

        case NVLINK_LINKSTATE_HS:
        {
            // check if link is in reset
            if (nvswitch_is_link_in_reset(device, link))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: link #%d is still in reset, cannot change link state\n",
                    __FUNCTION__, link->linkNumber);
                return -NVL_ERR_INVALID_STATE;
            }

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_STATE);
            link_state = DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, val);

            if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_ACTIVE)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : Link is already in Active mode (%s).\n",
                    __FUNCTION__, link->linkName);
                break;
            }
            else if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_INIT)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Link cannot be taken from INIT state to"
                    " Active mode for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return -NVL_ERR_INVALID_STATE;
            }
            else if (link_state == NV_NVLDL_TOP_LINK_STATE_STATE_SWCFG)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : Changing Link state to Active for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);

                val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _NEWSTATE, _ACTIVE, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _OLDSTATE_MASK, _DONTCARE, val);
                val = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _ACTION, _LTSSM_CHANGE, val);
                NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_CHANGE, val);
            }
            else
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Link is in invalid state"
                    " cannot set to active state (%s):(%s). (%x) (%x)\n",
                    __FUNCTION__, device->name, link->linkName, val, link_state);
                return -NVL_ERR_INVALID_STATE;
            }

            break;
        }

        case NVLINK_LINKSTATE_OFF:
        {
            _nvswitch_power_down_link_plls(link);

            if (nvswitch_lib_notify_client_events(device,
                        NVSWITCH_DEVICE_EVENT_PORT_DOWN) != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify PORT_DOWN event\n",
                             __FUNCTION__);
            }
            nvswitch_record_port_event(device, &(device->log_PORT_EVENTS), link->linkNumber, NVSWITCH_PORT_EVENT_TYPE_DOWN);

            break;
        }

        case NVLINK_LINKSTATE_RESET:
        {
            break;
        }

        case NVLINK_LINKSTATE_ENABLE_PM:
        {
            if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_YES)
            {
                status = nvswitch_minion_send_command(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_ENABLEPM, 0);

                if (status != NVL_SUCCESS)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: ENABLEPM CMD failed for link %d.\n",
                        __FUNCTION__, link->linkNumber);
                    return status;
                }
            }
            break;
        }

        case NVLINK_LINKSTATE_DISABLE_PM:
        {
            if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_YES)
            {
                status = nvswitch_minion_send_command(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_DISABLEPM, 0);

                if (status != NVL_SUCCESS)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: DISABLEPM CMD failed for link %d.\n",
                        __FUNCTION__, link->linkNumber);
                    return status;
                }
            }
            break;
        }

        case NVLINK_LINKSTATE_DISABLE_HEARTBEAT:
        {
            // NOP
            break;
        }

        case NVLINK_LINKSTATE_PRE_HS:
        {
            break;
        }

        case NVLINK_LINKSTATE_TRAFFIC_SETUP:
        {
            status = _nvswitch_init_link_post_active(link, flags);
            if (status != NVL_SUCCESS)
            {
                return status;
            }

            break;
        }

        case NVLINK_LINKSTATE_DISABLE_ERR_DETECT:
        {
            // Disable DL/PL interrupts
            _nvswitch_disable_dlpl_interrupts(link);
            break;
        }

        case NVLINK_LINKSTATE_LANE_DISABLE:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_LANEDISABLE, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : LANEDISABLE CMD failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }

        case NVLINK_LINKSTATE_LANE_SHUTDOWN:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_LANESHUTDOWN, 0);

            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : SHUTDOWN CMD failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }

        case NVLINK_LINKSTATE_INITPHASE1:
        {

           status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITPHASE1 failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CONFIGURATION_ERROR,
                    NVBIT32(link->linkNumber), INITPHASE1_ERROR);
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            // After INITPHASE1, apply NEA setting
            nvswitch_setup_link_loopback_mode(device, link->linkNumber);
            break;
        }

        case NVLINK_LINKSTATE_INITOPTIMIZE:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITOPTIMIZE, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITOPTIMIZE failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }

        case NVLINK_LINKSTATE_POST_INITOPTIMIZE:
        {
            // Poll for TRAINING_GOOD
            status  = nvswitch_minion_get_initoptimize_status_lr10(device, link->linkNumber);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                            "%s Error polling for INITOPTIMIZE TRAINING_GOOD. Link (%s):(%s)\n",
                            __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT32(link->linkNumber), INITOPTIMIZE_ERROR);
                return NV_ERR_NVLINK_TRAINING_ERROR;
            }

            // Send INITTL DLCMD
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITTL, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITTL failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT32(link->linkNumber), INITTL_ERROR);
                return NV_ERR_NVLINK_TRAINING_ERROR;
            }

            break;
        }

        case NVLINK_LINKSTATE_INITNEGOTIATE:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITNEGOTIATE, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITNEGOTIATE failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }

        case NVLINK_LINKSTATE_POST_INITNEGOTIATE:
        {
            // Poll for CONFIG_GOOD
            status  = nvswitch_minion_get_initnegotiate_status_lr10(device, link->linkNumber);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                            "%s Error polling for INITNEGOTIATE CONFIG_GOOD. Link (%s):(%s)\n",
                            __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CONFIGURATION_ERROR,
                    NVBIT32(link->linkNumber), INITNEGOTIATE_ERROR);
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }
            else
            {
                nvswitch_store_topology_information(device, link);
            }

            break;
        }

        default:
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s : Invalid mode specified.\n",
                __FUNCTION__);
            break;
        }
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_get_dl_link_mode_lr10
(
    nvlink_link *link,
    NvU64 *mode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 link_state;
    NvU32 val = 0;

    *mode = NVLINK_LINKSTATE_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // check if links are in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        *mode = NVLINK_LINKSTATE_RESET;
        return NVL_SUCCESS;
    }

    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_STATE);

    link_state = DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, val);

    switch (link_state)
    {
        case NV_NVLDL_TOP_LINK_STATE_STATE_INIT:
            *mode = NVLINK_LINKSTATE_OFF;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_HWCFG:
            *mode = NVLINK_LINKSTATE_DETECT;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_SWCFG:
            *mode = NVLINK_LINKSTATE_SAFE;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_ACTIVE:
            *mode = NVLINK_LINKSTATE_HS;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_FAULT:
            *mode = NVLINK_LINKSTATE_FAULT;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_RCVY_AC:
        case NV_NVLDL_TOP_LINK_STATE_STATE_RCVY_RX:
            *mode = NVLINK_LINKSTATE_RECOVERY;
            break;
        default:
            *mode = NVLINK_LINKSTATE_OFF;
            break;
    }

    return NVL_SUCCESS;
}

void
nvswitch_corelib_get_uphy_load_lr10
(
    nvlink_link *link,
    NvBool *bUnlocked
)
{
    *bUnlocked = NV_FALSE;
}

NvlStatus
nvswitch_corelib_set_tl_link_mode_lr10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvlStatus       status = NVL_SUCCESS;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    if (nvswitch_does_link_need_termination_enabled(device, link))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: link #% is connected to a disabled remote end. Skipping TL link mode request!\n", __FUNCTION__, link->linkNumber);

        // return SUCCESS to avoid errors being propogated
        return NVL_SUCCESS;
    }

    switch (mode)
    {
        case NVLINK_LINKSTATE_RESET:
        {
            // perform TL reset
            NVSWITCH_PRINT(device, INFO,
                "%s: Performing TL Reset on link %d\n",
                __FUNCTION__, link->linkNumber);

            status = nvswitch_request_tl_link_state_lr10(link,
                NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET,
                flags == NVLINK_STATE_CHANGE_SYNC);

            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: NvLink Reset has failed for link %d\n",
                    __FUNCTION__, link->linkNumber);
                return status;
            }
            break;
        }

        default:
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s : Invalid mode specified.\n",
                __FUNCTION__);
            break;
        }
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_get_tl_link_mode_lr10
(
    nvlink_link *link,
    NvU64 *mode
)
{
#if defined(INCLUDE_NVLINK_LIB)

    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 link_state;
    NvU32 val = 0;

    *mode = NVLINK_LINKSTATE_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // check if links are in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        *mode = NVLINK_LINKSTATE_RESET;
        return NVL_SUCCESS;
    }

    // Read state from NVLIPT HW
    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
            _NVLIPT_LNK, _CTRL_LINK_STATE_STATUS);

    link_state = DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_STATUS, _CURRENTLINKSTATE,
            val);

    switch(link_state)
    {
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_ACTIVE:
            *mode = NVLINK_LINKSTATE_HS;
            break;

        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_L2:
            *mode = NVLINK_LINKSTATE_SLEEP;
            break;

        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_CONTAIN:
            *mode = NVLINK_LINKSTATE_CONTAIN;
            break;

        default:
            // Currently, only ACTIVE, L2 and CONTAIN states are supported
            return NVL_ERR_INVALID_STATE;
            break;
    }

#endif

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_set_tx_mode_lr10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 tx_sublink_state;
    NvU32 val;
    NvlStatus status = NVL_SUCCESS;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    if (nvswitch_does_link_need_termination_enabled(device, link))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: link #% is connected to a disabled remote end. Skipping TX mode request!\n", __FUNCTION__, link->linkNumber);

        // return SUCCESS to avoid errors being propogated
        return NVL_SUCCESS;
    }

    // check if link is in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d is still in reset, cannot change sub-link state\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_ERR_INVALID_STATE;
    }

    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);

    tx_sublink_state = DRF_VAL(_NVLDL_TX, _SLSM_STATUS_TX, _PRIMARY_STATE, val);

    // Check if Sublink State Machine is ready to accept a sublink change request.
    status = nvswitch_poll_sublink_state(device, link);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : SLSM not ready to accept a state change request for(%s):(%s).\n",
            __FUNCTION__, device->name, link->linkName);
        return status;
    }

    switch (mode)
    {
        case NVLINK_SUBLINK_STATE_TX_COMMON_MODE:
        {
            val = _nvswitch_init_dl_pll(link);
            if (val != NVL_SUCCESS)
            {
                return val;
            }

            break;
        }

        case NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE:
        {
            // Not applicable for NV IP
            break;
        }

        case NVLINK_SUBLINK_STATE_TX_DATA_READY:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITDLPL, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: INITNVLDL CMD failed for link %d.\n",
                    __FUNCTION__, link->linkNumber);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_INIT_ERROR,
                    NVBIT32(link->linkNumber), INITDLPL_ERROR);
                return NV_ERR_NVLINK_INIT_ERROR;
            }

            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITLANEENABLE, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: INITLANEENABLE CMD failed for link %d.\n",
                    __FUNCTION__, link->linkNumber);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_INIT_ERROR, NVBIT32(link->linkNumber), INITLANEENABLE_ERROR);
                return NV_ERR_NVLINK_INIT_ERROR;
            }

            break;
        }

        case NVLINK_SUBLINK_STATE_TX_PRBS_EN:
        {
            // Not needed with ALT
            break;
        }

        case NVLINK_SUBLINK_STATE_TX_POST_HS:
        {
          // NOP: In general, there is no point to downgrade *_PRBS_* and *_SCRAM_* values.
          break;
        }

        case NVLINK_SUBLINK_STATE_TX_EQ:
        {
            //TODO: To be implemented
            break;
        }

        case NVLINK_SUBLINK_STATE_TX_HS:
        {
            // Not needed with ALT
            break;
        }

        case NVLINK_SUBLINK_STATE_TX_SAFE:
        {
            if (tx_sublink_state == NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_SAFE)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : TX already in Safe mode for  (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                break;
            }

            NVSWITCH_PRINT(device, INFO,
                  "%s : Changing TX sublink state to Safe mode for (%s):(%s).\n",
                  __FUNCTION__, device->name, link->linkName);

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _NEWSTATE, _SAFE, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _SUBLINK, _TX, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _ACTION, _SLSM_CHANGE, val);
            NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE, val);

            status = nvswitch_poll_sublink_state(device, link);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Error while changing TX sublink to Safe Mode for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }

        case NVLINK_SUBLINK_STATE_TX_OFF:
        {
            if (tx_sublink_state == NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_OFF)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : TX already OFF (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                break;
            }
            else if (tx_sublink_state == NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_HS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : TX cannot be taken from HS to OFF directly for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return -NVL_ERR_GENERIC;
            }

            NVSWITCH_PRINT(device, INFO,
                "%s : Changing TX sublink state to OFF for (%s):(%s).\n",
                __FUNCTION__, device->name, link->linkName);

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _COUNTDOWN, _IMMEDIATE, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _NEWSTATE, _OFF, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _SUBLINK, _TX, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _ACTION, _SLSM_CHANGE, val);
            NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE, val);

            status = nvswitch_poll_sublink_state(device, link);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Error while changing TX sublink to off Mode for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }
            break;
        }

        default:
            NVSWITCH_PRINT(device, ERROR,
                 "%s : Invalid TX sublink mode specified.\n",
                __FUNCTION__);
            break;
    }

    return status;
}

NvlStatus
nvswitch_corelib_get_tx_mode_lr10
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 tx_sublink_state;
    NvU32 data = 0;

    *mode = NVLINK_SUBLINK_STATE_TX_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // check if link is in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        *mode = NVLINK_SUBLINK_STATE_TX_OFF;
        return NVL_SUCCESS;
    }
    
    data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);

    tx_sublink_state = DRF_VAL(_NVLDL_TX, _SLSM_STATUS_TX, _PRIMARY_STATE, data);

    // Return NVLINK_SUBLINK_SUBSTATE_TX_STABLE for sub-state
    *subMode = NVLINK_SUBLINK_SUBSTATE_TX_STABLE;

    switch (tx_sublink_state)
    {
        case NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_EIGHTH:
            *mode = NVLINK_SUBLINK_STATE_TX_SINGLE_LANE;
            break;

        case NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_HS:
            *mode = NVLINK_SUBLINK_STATE_TX_HS;
            break;

        case NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_TRAIN:
            *mode = NVLINK_SUBLINK_STATE_TX_TRAIN;
            break;

        case NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_SAFE:
            *mode = NVLINK_SUBLINK_STATE_TX_SAFE;
            break;

        case NV_NVLDL_TX_SLSM_STATUS_TX_PRIMARY_STATE_OFF:
            *mode = NVLINK_SUBLINK_STATE_TX_OFF;
            break;

        default:
            *mode = NVLINK_SUBLINK_STATE_TX_OFF;
            break;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_set_rx_mode_lr10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 rx_sublink_state;
    NvU32 val;
    NvlStatus status = NVL_SUCCESS;
    NvU32 delay_ns;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    if (nvswitch_does_link_need_termination_enabled(device, link))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: link #% is connected to a disabled remote end. Skipping RX mode request!\n", __FUNCTION__, link->linkNumber);

        // return SUCCESS to avoid errors being propogated
        return NVL_SUCCESS;
    }

    // check if link is in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d is still in reset, cannot change sub-link state\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_ERR_INVALID_STATE;
    }


    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _SLSM_STATUS_RX);

    rx_sublink_state = DRF_VAL(_NVLDL_RX, _SLSM_STATUS_RX, _PRIMARY_STATE, val);

    // Check if Sublink State Machine is ready to accept a sublink change request.
    status = nvswitch_poll_sublink_state(device, link);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : SLSM not ready to accept a state change request for(%s):(%s).\n",
            __FUNCTION__, device->name, link->linkName);
        return status;
    }

    switch (mode)
    {
        case NVLINK_SUBLINK_STATE_RX_HS:
            break;

        case NVLINK_SUBLINK_STATE_RX_SAFE:
            break;

        case NVLINK_SUBLINK_STATE_RX_OFF:
        {
            if (rx_sublink_state == NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_OFF)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s : RX already OFF (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                break;
            }
            else if (rx_sublink_state == NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_HS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : RX cannot be taken from HS to OFF directly for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                status = -NVL_ERR_GENERIC;
                return status;
            }

            NVSWITCH_PRINT(device, INFO,
                "%s : Changing RX sublink state to OFF for (%s):(%s).\n",
                __FUNCTION__, device->name, link->linkName);

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _COUNTDOWN, _IMMEDIATE, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _OLDSTATE_MASK, _DONTCARE, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _NEWSTATE, _OFF, val);
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _SUBLINK, _RX, val);

            // When changing RX sublink state use FORCE, otherwise it will fault.
            val = FLD_SET_DRF(_NVLDL_TOP, _SUBLINK_CHANGE, _ACTION, _SLSM_FORCE, val);
            NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _SUBLINK_CHANGE, val);

            NVSWITCH_PRINT(device, INFO,
                "%s : NV_NVLDL_TOP_SUBLINK_CHANGE = 0x%08x\n", __FUNCTION__, val);

            status = nvswitch_poll_sublink_state(device, link);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Error while changing RX sublink to Off Mode for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }
            break;
        }

        case NVLINK_SUBLINK_STATE_RX_RXCAL:
        {
            // Enable RXCAL in CFG_CTL_6, Delay 200us (bug 2551877), and check CFG_STATUS_0 for RXCAL_DONE=1.
            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLPHYCTL_COMMON, _CFG_CTL_6);
            val = FLD_SET_DRF(_NVLPHYCTL_COMMON, _CFG_CTL_6, _RXCAL , _ON, val);
            NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLPHYCTL_COMMON, _CFG_CTL_6, val);

            if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
            {
                delay_ns = NVSWITCH_INTERVAL_1SEC_IN_NS;
            }
            else
            {
                delay_ns = 200 * NVSWITCH_INTERVAL_1USEC_IN_NS;
            }

            NVSWITCH_NSEC_DELAY(delay_ns);

            val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLPHYCTL_COMMON, _CFG_STATUS_0);
            if (!FLD_TEST_DRF_NUM(_NVLPHYCTL_COMMON, _CFG_STATUS_0, _RXCAL_DONE, 0x1, val))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Timeout while waiting for RXCAL_DONE on link %d.\n",
                    __FUNCTION__, link->linkNumber);
                return -NVL_ERR_GENERIC;
            }
            break;
        }

        case NVLINK_SUBLINK_STATE_RX_INIT_TERM:
        {
            // Invoke MINION routine to enable RX Termination
            status = nvswitch_minion_set_rx_term_lr10(device, link->linkNumber);

            if (status != NV_OK)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : Error while setting RX INIT_TERM for (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                return status;
            }

            break;
        }


        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s : Invalid RX sublink mode specified.\n",
                __FUNCTION__);
            break;
    }

    return status;
}

NvlStatus
nvswitch_corelib_get_rx_mode_lr10
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 rx_sublink_state;
    NvU32 data = 0;

    *mode = NVLINK_SUBLINK_STATE_RX_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // check if link is in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        *mode = NVLINK_SUBLINK_STATE_RX_OFF;
        return NVL_SUCCESS;
    }
    
    data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _SLSM_STATUS_RX);

    rx_sublink_state = DRF_VAL(_NVLDL_RX, _SLSM_STATUS_RX, _PRIMARY_STATE, data);

    // Return NVLINK_SUBLINK_SUBSTATE_RX_STABLE for sub-state
    *subMode = NVLINK_SUBLINK_SUBSTATE_RX_STABLE;

    switch (rx_sublink_state)
    {
        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_HS:
            *mode = NVLINK_SUBLINK_STATE_RX_HS;
            break;

        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_TRAIN:
            *mode = NVLINK_SUBLINK_STATE_RX_TRAIN;
            break;

        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_SAFE:
            *mode = NVLINK_SUBLINK_STATE_RX_SAFE;
            break;

        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_EIGHTH:
            *mode = NVLINK_SUBLINK_STATE_RX_SINGLE_LANE;
            break;

        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_OFF:
            *mode = NVLINK_SUBLINK_STATE_RX_OFF;
            break;

        default:
            *mode = NVLINK_SUBLINK_STATE_RX_OFF;
            break;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_set_rx_detect_lr10
(
    nvlink_link *link,
    NvU32 flags
)
{
    NvlStatus status;
    nvswitch_device *device = link->dev->pDevInfo;

    if (nvswitch_does_link_need_termination_enabled(device, link))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: link #% is connected to a disabled remote end. Skipping RxDet request!\n", __FUNCTION__, link->linkNumber);

        // return SUCCESS to avoid errors being propogated
        return NVL_SUCCESS;
    }

    status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_TURING_RXDET, 0);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Set RXDET failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_get_rx_detect_lr10
(
    nvlink_link *link
)
{
    NvlStatus status;
    nvswitch_device *device = link->dev->pDevInfo;

    status = nvswitch_minion_get_rxdet_status_lr10(device, link->linkNumber);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Get RXDET failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        return status;
    }
    return NVL_SUCCESS;
}

void
nvswitch_corelib_training_complete_lr10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;

    nvswitch_init_dlpl_interrupts(link);

    _nvswitch_configure_reserved_throughput_counters(link);

    if (nvswitch_lib_notify_client_events(device,
                NVSWITCH_DEVICE_EVENT_PORT_UP) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify PORT_UP event\n",
                     __FUNCTION__);
    }
    nvswitch_record_port_event(device, &(device->log_PORT_EVENTS), link->linkNumber, NVSWITCH_PORT_EVENT_TYPE_UP);

}

NvlStatus
nvswitch_wait_for_tl_request_ready_lr10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32     linkRequest;
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    NvU32 linkStatus, linkErr;
#endif

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLIPT_LNK, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_BAD_ARGS;
    }

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * 400, &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        linkRequest = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
                NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_REQUEST);

        if (FLD_TEST_DRF_NUM(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _READY, 1, linkRequest))
        {
            return NVL_SUCCESS;
        }

        nvswitch_os_sleep(1);
    }
    while(keepPolling);

    //
    // NVSWITCH_PRINT is not defined for release builds,
    // so this keeps compiler happy
    //
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    linkStatus  = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
            NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_STATUS);
    linkErr     = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
            NVLIPT_LNK , _NVLIPT_LNK , _ERR_STATUS_0);
#endif

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for TL link state ready on link #%d! "
              "NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST = 0x%x, "
              "NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS = 0x%x, "
              "NV_NVLIPT_LNK_ERR_STATUS_0 = 0x%x\n",
        __FUNCTION__, link->linkNumber, linkRequest, linkStatus, linkErr);

    return -NVL_ERR_GENERIC;
}

NvlStatus
nvswitch_request_tl_link_state_lr10
(
    nvlink_link *link,
    NvU32        tlLinkState,
    NvBool       bSync
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvlStatus status = NVL_SUCCESS;
    NvU32 linkStatus;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLIPT_LNK, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // Wait for the TL link state register to report ready
    status = nvswitch_wait_for_tl_request_ready_lr10(link);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    // Request RESET state through CTRL_LINK_STATE_REQUEST
    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST,
            DRF_NUM(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _REQUEST, tlLinkState));

    if (bSync)
    {
        // Wait for the TL link state register to complete
        status = nvswitch_wait_for_tl_request_ready_lr10(link);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        // Check for state requested
        linkStatus  = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
                NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_STATUS);

        if (DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_STATUS, _CURRENTLINKSTATE, linkStatus) !=
                    tlLinkState)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: TL link state request to state 0x%x for link #%d did not complete!\n",
                __FUNCTION__, tlLinkState, link->linkNumber);
            return -NVL_ERR_GENERIC;
        }
    }

    return status;

}

void
nvswitch_execute_unilateral_link_shutdown_lr10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return;
    }

    //
    // Perform unilateral shutdown
    // This follows "Unilateral variant" from
    // NVLink 3.x Shutdown (confluence page ID: 164573291)
    //
    // Status is explicitly ignored here since we are required to soldier-on
    // in this scenario
    //
    nvswitch_corelib_set_dl_link_mode_lr10(link, NVLINK_LINKSTATE_DISABLE_PM, 0);
    nvswitch_corelib_set_dl_link_mode_lr10(link, NVLINK_LINKSTATE_DISABLE_ERR_DETECT, 0);
    nvswitch_corelib_set_dl_link_mode_lr10(link, NVLINK_LINKSTATE_LANE_DISABLE, 0);
    nvswitch_corelib_set_dl_link_mode_lr10(link, NVLINK_LINKSTATE_OFF, 0);
}

static NvU32
_nvswitch_get_nvlink_linerate_lr10
(
    nvswitch_device *device,
    NvU32            val
)
{
    NvU32  lineRate = 0;
    switch (val)
    {
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_16G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_16_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_20G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_20_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_25G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_25_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_32G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_32_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_40G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_40_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_50G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_50_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_53_12500G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_53_12500_GBPS;
            break;
        default:
            NVSWITCH_PRINT(device, SETUP, "%s:ERROR LINE_RATE = 0x%x requested by regkey\n",
                       __FUNCTION__, lineRate);
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_ILLEGAL_LINE_RATE;
    }
    return lineRate;
}

void
nvswitch_setup_link_system_registers_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{   
    NvU32 regval = 0;
    NvU32 fldval = 0;
    NvU32 lineRate = 0;
    NVLINK_CONFIG_DATA_LINKENTRY *vbios_link_entry = NULL;
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config;
    NvU32 base_entry;

    bios_config = nvswitch_get_bios_nvlink_config(device);
    if ((bios_config == NULL) || (bios_config->bit_address == 0))
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: VBIOS NvLink configuration table not found\n",
            __FUNCTION__);
    }

    //
    // Identify the valid link entry to update. If not, proceed with the default settings
    //
    if ((bios_config == NULL) || (bios_config->bit_address == 0))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: No override with VBIOS - VBIOS NvLink configuration table not found\n",
            __FUNCTION__);
    }
    else
    {
        base_entry = bios_config->link_base_entry_assigned;

        vbios_link_entry = &bios_config->link_vbios_entry[base_entry][link->linkNumber];
    }

    regval = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                                           _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL);

    // LINE_RATE SYSTEM register
    if (device->regkeys.nvlink_speed_control != NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_DEFAULT)
    {
        lineRate = _nvswitch_get_nvlink_linerate_lr10(device, device->regkeys.nvlink_speed_control);
        regval   = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL,
                                    _LINE_RATE, lineRate, regval);
        NVSWITCH_PRINT(device, SETUP, "%s: LINE_RATE = 0x%x requested by regkey\n",
                       __FUNCTION__, lineRate);
    }

    // REFERENCE_CLOCK_MODE SYSTEM register
    if (device->regkeys.reference_clock_mode != NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_DEFAULT)
    {
        regval   = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL,
                                    _REFERENCE_CLOCK_MODE, device->regkeys.reference_clock_mode, regval);
        NVSWITCH_PRINT(device, SETUP, "%s: REFERENCE_CLOCK_MODE = 0x%x requested by regkey\n",
                       __FUNCTION__, device->regkeys.reference_clock_mode);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CLK_CTRL, _REFERENCE_CLOCK_MODE,
                                 DRF_VAL(_NVLINK_VBIOS,_PARAM3,_REFERENCE_CLOCK_MODE, vbios_link_entry->nvLinkparam3),
                                 regval);
    }

    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLIPT_LNK,
                        _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL, regval);

    // TXTRAIN SYSTEM register
    regval = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                                     _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL);

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _FOM_FORMAT,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_FOM_FORMAT_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: FOM_FORMAT = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_FOM_FORMAT, fldval, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _TXTRAIN_FOM_FORMAT,
                                    DRF_VAL(_NVLINK_VBIOS,_PARAM5,_TXTRAIN_FOM_FORMAT, vbios_link_entry->nvLinkparam5),
                                    regval);
    }

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _OPTIMIZATION_ALGORITHM,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: OPTIMIZATION_ALGORITHM = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_OPTIMIZATION_ALGORITHM, fldval, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _TXTRAIN_OPTIMIZATION_ALGORITHM,
                                 vbios_link_entry->nvLinkparam4, regval);
    }

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _ADJUSTMENT_ALGORITHM,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_ADJUSTMENT_ALGORITHM_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: ADJUSTMENT_ALGORITHM = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_ADJUSTMENT_ALGORITHM, fldval, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _TXTRAIN_ADJUSTMENT_ALGORITHM,
                                     DRF_VAL(_NVLINK_VBIOS,_PARAM5,_TXTRAIN_ADJUSTMENT_ALGORITHM, vbios_link_entry->nvLinkparam5),
                                     regval);
    }

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _MINIMUM_TRAIN_TIME_MANTISSA,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_MANTISSA_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: MINIMUM_TRAIN_TIME_MANTISSA = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_MINIMUM_TRAIN_TIME_MANTISSA, fldval, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _TXTRAIN_MINIMUM_TRAIN_TIME_MANTISSA,
                                 DRF_VAL(_NVLINK_VBIOS,_PARAM6,_TXTRAIN_MINIMUM_TRAIN_TIME_MANTISSA, vbios_link_entry->nvLinkparam6),
                                 regval);
    }

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _MINIMUM_TRAIN_TIME_EXPONENT,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_EXPONENT_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: MINIMUM_TRAIN_TIME_EXPONENT = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_MINIMUM_TRAIN_TIME_EXPONENT, fldval, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _TXTRAIN_MINIMUM_TRAIN_TIME_EXPONENT,
                                 DRF_VAL(_NVLINK_VBIOS,_PARAM6,_TXTRAIN_MINIMUM_TRAIN_TIME_EXPONENT, vbios_link_entry->nvLinkparam6),
                                 regval);
    }

    // AC vs DC mode SYSTEM register
    if (link->ac_coupled)
    {
        //
        // In NVL3.0, ACMODE is handled by MINION in the INITPHASE1 command
        // Here we just setup the register with the proper info
        //
        NVSWITCH_PRINT(device, SETUP, "%s: AC_DC_MODE = 0x%x\n",
                       __FUNCTION__, DRF_VAL(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL,
                                                _AC_DC_MODE, regval));
        regval = FLD_SET_DRF(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _AC_DC_MODE, _AC, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _AC_DC_MODE,
                                    DRF_VAL(_NVLINK_VBIOS, _PARAM0, _ACDC_MODE, vbios_link_entry->nvLinkparam0),
                                    regval);
    }

    if (device->regkeys.block_code_mode != NV_SWITCH_REGKEY_BLOCK_CODE_MODE_DEFAULT)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: BLOCK_CODE_MODE = 0x%x requested by regkey\n",
                       __FUNCTION__, device->regkeys.block_code_mode);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _BLOCK_CODE_MODE, device->regkeys.block_code_mode, regval);
    }
    else if (vbios_link_entry != NULL)
    {
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _BLOCK_CODE_MODE,
                                    DRF_VAL(_NVLINK_VBIOS, _PARAM3, _CLOCK_MODE_BLOCK_CODE, vbios_link_entry->nvLinkparam3),
                                    regval);
    }

    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLIPT_LNK,
                            _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL, regval);

    // Disable L2 (Bug 3176196)
    regval = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SYSTEM_LINK_AN1_CTRL);
    regval = FLD_SET_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_AN1_CTRL, _PWRM_L2_ENABLE, _DISABLE, regval);
    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SYSTEM_LINK_AN1_CTRL, regval);

    // SW WAR: Bug 3364420
    nvswitch_apply_recal_settings(device, link);

    return;
}

void
nvswitch_load_link_disable_settings_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{   
    NvU32 val;
    NVLINK_CONFIG_DATA_LINKENTRY *vbios_link_entry = NULL;
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config;
    NvlStatus status;
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    bios_config = nvswitch_get_bios_nvlink_config(device);
    if ((bios_config == NULL) || (bios_config->bit_address == 0))
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: VBIOS NvLink configuration table not found\n",
            __FUNCTION__);
    }

    // SW CTRL - clear out LINK_DISABLE on driver load
    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SW_LINK_MODE_CTRL);
    val = FLD_SET_DRF(_NVLIPT_LNK, _CTRL_SW_LINK_MODE_CTRL, _LINK_DISABLE,
                      _ENABLED, val);
    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SW_LINK_MODE_CTRL, val);

    //
    // SYSTEM CTRL
    // If the SYSTEM_CTRL setting had been overidden by another entity,
    // it should also be locked, so this write would not take effect.
    //
    if (bios_config != NULL)
    {
        vbios_link_entry = &bios_config->link_vbios_entry[bios_config->link_base_entry_assigned][link->linkNumber];
    }

    val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SYSTEM_LINK_MODE_CTRL);

    if ((vbios_link_entry != NULL) &&
         (FLD_TEST_DRF(_NVLINK_VBIOS,_PARAM0, _LINK, _DISABLE, vbios_link_entry->nvLinkparam0)))
    {
        if (!nvswitch_is_link_in_reset(device, link))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: link #%d is not in reset, cannot set LINK_DISABLE\n",
                __FUNCTION__, link->linkNumber);
            return;
        }

        status = nvswitch_link_termination_setup(device, link);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to enable termination on link #%d\n", __FUNCTION__, link->linkNumber);
            return;
        }
        // add link to disabledRemoteEndLinkMask
        chip_device->disabledRemoteEndLinkMask |= NVBIT64(link->linkNumber);

        return;
    }
    else
    {
        val = FLD_SET_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_MODE_CTRL, _LINK_DISABLE,
                          _ENABLED, val);
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber,
                NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SYSTEM_LINK_MODE_CTRL, val);
    }

    return;
}

void 
nvswitch_reset_persistent_link_hw_state_lr10
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    // Not Implemented for LR10
}

void
nvswitch_apply_recal_settings_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    // Not supported on LR10
    return;
}

NvlStatus
nvswitch_launch_ALI_link_training_lr10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvBool           bSync
)
{
    return NVL_ERR_NOT_IMPLEMENTED;
}

NvlStatus
nvswitch_reset_and_train_link_lr10
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    return NVL_ERR_NOT_IMPLEMENTED;
}

NvBool
nvswitch_does_link_need_termination_enabled_lr10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
#if defined(INCLUDE_NVLINK_LIB)
    NvU32 i;
    NvU32 physicalId;
    lr10_device *chip_device;
    NvU32 numNvswitches;
    NvlStatus status;

    physicalId = nvswitch_read_physical_id(device);
    chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    if (chip_device == NULL)
    {
      NVSWITCH_PRINT(device, ERROR,
        "%s: Failed to get lr10 chip device!\n", __FUNCTION__);
      return NV_FALSE;
    }

    //
    // If disabledRemoteEndLinkMask has not been cached then
    // using the switch's physicalId search nvswitchDisconnetedRemoteLinkMasks
    // til a match is found then copy out the linkMask to the chip_device
    // Only run this operation if there is a registed device with a reduced
    // nvlink config
    //
    if (chip_device->bDisabledRemoteEndLinkMaskCached == NV_FALSE)
    {
        chip_device->disabledRemoteEndLinkMask = 0;
        if (nvlink_lib_is_registerd_device_with_reduced_config())
        {
            for (i = 0; i < NUM_SWITCH_WITH_DISCONNETED_REMOTE_LINK; ++i)
            {
                if (nvswitchDisconnetedRemoteLinkMasks[i].switchPhysicalId == physicalId)
                {
                    chip_device->disabledRemoteEndLinkMask |=
                                    nvswitchDisconnetedRemoteLinkMasks[i].accessLinkMask;

                    status = nvlink_lib_return_device_count_by_type(NVLINK_DEVICE_TYPE_NVSWITCH, &numNvswitches);
                    if (status != NVL_SUCCESS)
                    {
                        NVSWITCH_PRINT(device, ERROR,
                                        "%s: Failed to get nvswitch device count!\n", __FUNCTION__);
                        break;
                    }
                    
                    if (numNvswitches <= NVSWITCH_NUM_DEVICES_PER_DELTA_LR10)
                    {
                        chip_device->disabledRemoteEndLinkMask |= 
                                    nvswitchDisconnetedRemoteLinkMasks[i].trunkLinkMask;
                    }
                    break;
                }
            }
        }

        chip_device->bDisabledRemoteEndLinkMaskCached = NV_TRUE;
    }

    // return NV_TRUE if the link is inside of  disabledRemoteEndLinkMask
    return ((BIT64(link->linkNumber) & chip_device->disabledRemoteEndLinkMask) != 0);
#else
    return NV_FALSE;
#endif //defined(INCLUDE_NVLINK_LIB)
}

NvlStatus
nvswitch_link_termination_setup_lr10
(
    nvswitch_device *device,
    nvlink_link* link
)
{
    NvlStatus status;
    NvU32 linkId = link->linkNumber;

    // Sanity check
    if ((link == NULL) ||
        (linkId >= NVSWITCH_NVLINK_MAX_LINKS) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, linkId))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Link %d is invalid!\n", __FUNCTION__, linkId);
        return NVL_BAD_ARGS;
    }

    // Sanity check nvlink version
    if (link->version != NVLINK_DEVICE_VERSION_30)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Link %d: only nvlink version 3.0 can run the termination setup\n",
                     __FUNCTION__, linkId);
        return NVL_BAD_ARGS;
    }

    // Send INITPHASE1 to the link
    status = nvswitch_minion_send_command_lr10(device, link->linkNumber,
                    NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send initphase1 to link %d", __FUNCTION__, linkId);
        return NVL_ERR_INVALID_STATE;
    }

    // Send INITRXTXTERM to the link
    nvswitch_minion_send_command_lr10(device, link->linkNumber,
        NV_MINION_NVLINK_DL_CMD_COMMAND_INITRXTXTERM, 0);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to send INITRXTXTERM to link %d", __FUNCTION__, linkId);
    }

    NVSWITCH_PRINT(device, INFO,
        "%s: enabled termination for switchPhysicalId %d link# %d\n",
        __FUNCTION__, nvswitch_read_physical_id(device), linkId);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_link_l1_capability_lr10
(
    nvswitch_device *device,
    NvU32 linkNum,
    NvBool *isL1Capable
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_link_l1_threshold_lr10
(
    nvswitch_device *device,
    NvU32 linkNum,
    NvU32 *lpThreshold
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_set_link_l1_threshold_lr10
(
    nvlink_link *link,
    NvU32 lpEntryThreshold
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

