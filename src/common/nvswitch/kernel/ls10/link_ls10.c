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

#include "nvlink_export.h"

#include "export_nvswitch.h"
#include "common_nvswitch.h"
#include "regkey_nvswitch.h"
#include "ls10/ls10.h"
#include "nvswitch/ls10/dev_nvldl_ip_addendum.h"
#include "cci/cci_nvswitch.h"

#include "nvswitch/ls10/dev_nvldl_ip.h"
#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_nvlphyctl_ip.h"
#include "nvswitch/ls10/dev_nvltlc_ip.h"
#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_minion_ip_addendum.h"
#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_nvlipt_ip.h"
#include "nvswitch/ls10/dev_nport_ip.h"
#include "nvswitch/ls10/dev_minion_ip_addendum.h" 
#include "ls10/minion_nvlink_defines_public_ls10.h"

#define NV_NVLINK_TLREQ_TIMEOUT_ACTIVE     10000
#define NV_NVLINK_TLREQ_TIMEOUT_SHUTDOWN   10
#define NV_NVLINK_TLREQ_TIMEOUT_RESET      4
#define NV_NVLINK_TLREQ_TIMEOUT_L2         5

static void
_nvswitch_configure_reserved_throughput_counters
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNum = link->linkNumber;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLTLC, link->linkNumber))
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
    NVSWITCH_LINK_WR32_IDX_LS10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, 0,
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)           |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA)      |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _ENABLE, _ENABLE));

    // Tx0 config
    NVSWITCH_LINK_WR32_IDX_LS10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, 0,
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)           |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA)      |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _ENABLE, _ENABLE));

    // Rx2 config
    NVSWITCH_LINK_WR32_IDX_LS10(device, linkNum, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, 2,
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)           |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _HEAD)      |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _AE)        |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _BE)        |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA)      |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT) |
        DRF_DEF(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL_0, _ENABLE, _ENABLE));

    // Tx2 config
    NVSWITCH_LINK_WR32_IDX_LS10(device, linkNum, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, 2,
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _UNIT, _FLITS)           |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _HEAD)      |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _AE)        |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _BE)        |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _FLITFILTER, _DATA)      |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _VCSETFILTERMODE, _INIT) |
        DRF_DEF(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL_0, _ENABLE, _ENABLE));
}

void
nvswitch_program_l1_scratch_reg_ls10
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    NvU32 scrRegVal;
    NvU32 tempRegVal;

    // Read L1 register and store initial/VBIOS L1 Threshold Value in Scratch register
    tempRegVal = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _PWRM_L1_ENTER_THRESHOLD);
 
    scrRegVal = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _SCRATCH_WARM);

    // Update the scratch register value only if it has not been written to before
    if (scrRegVal == NV_NVLIPT_LNK_SCRATCH_WARM_DATA_INIT)
    {
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _SCRATCH_WARM, tempRegVal);
    }
}

#define BUG_3797211_LS10_VBIOS_VERSION     0x9610410000

void
nvswitch_init_lpwr_regs_ls10
(
    nvlink_link *link
)
{
    NvlStatus status;
    nvswitch_device *device;

    if (link == NULL)
    {
        return;
    }

    device = link->dev->pDevInfo;

    status = nvswitch_ctrl_set_link_l1_threshold_ls10(link, device->regkeys.lp_threshold);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to set L1 Threshold\n",
                       __FUNCTION__);
    }
}

void
nvswitch_corelib_training_complete_ls10
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

    return;
}

NvlStatus
nvswitch_wait_for_tl_request_ready_ls10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NVSWITCH_MINION_ALI_DEBUG_REGISTERS params;
    NvU32 nvldlErrCntl, nvldlTopLinkState, nvldlTopIntr, linkStateRequest;
    NvlStatus status = nvswitch_wait_for_tl_request_ready_lr10(link);

    
    if(status == -NVL_ERR_GENERIC)
    {
        linkStateRequest = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_REQUEST);
        nvldlErrCntl  = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                NVLDL, _NVLDL_RX_RXSLSM , _ERR_CNTL);
        nvldlTopLinkState = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                NVLDL, _NVLDL_TOP , _LINK_STATE);
        nvldlTopIntr = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                NVLDL, _NVLDL_TOP , _INTR);
   
        nvswitch_minion_get_ali_debug_registers_ls10(device, link, &params);

        NVSWITCH_PRINT(device, ERROR,
            "%s: Ali Training failed on link #%d!:\n"
                "NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST = 0x%x, "
                "NV_NVLDL_RXSLSM_ERR_CNTL = 0x%x,\n"
                "NV_NVLDL_TOP_LINK_STATE = 0x%x,\n"
                "NV_NVLDL_TOP_INTR = 0x%x,\n"
                "Minion DLSTAT MN00 = 0x%x\n"
                "Minion DLSTAT UC01 = 0x%x\n"
                "Minion DLSTAT UC01 = 0x%x\n",
            __FUNCTION__, link->linkNumber,
            linkStateRequest,
            nvldlErrCntl, nvldlTopLinkState, nvldlTopIntr,
            params.dlstatMn00, params.dlstatUc01, params.dlstatLinkIntr);
        
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_NVLIPT_LNK_ALI_TRAINING_FAIL,
                            "ALI Training failure. Info 0x%x%x%x%x%x%x%x\n", 
                            params.dlstatMn00, params.dlstatUc01, params.dlstatLinkIntr,
                            nvldlTopLinkState, nvldlTopIntr, nvldlErrCntl, linkStateRequest);
    }
    return status;
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
        NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CLOCK_ERROR, NVBIT64(link->linkNumber), INITPLL_ERROR);
        return NV_ERR_NVLINK_CLOCK_ERROR;
    }

    status = nvswitch_minion_send_command(device, link->linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHY, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: INITPHY failed for link %d.\n",
            __FUNCTION__, link->linkNumber);
        NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_INIT_ERROR, NVBIT64(link->linkNumber), INITPHY_ERROR);
        return NV_ERR_NVLINK_INIT_ERROR;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_set_tx_mode_ls10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 val;
    NvlStatus status = NVL_SUCCESS;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    // check if link is in reset
    if (nvswitch_is_link_in_reset(device, link))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d is still in reset, cannot change sub-link state\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_ERR_INVALID_STATE;
    }

    val = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);

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

        default:
        {
           status = nvswitch_corelib_set_tx_mode_lr10(link, mode, flags);
        }
    }

    return status;
}

NvU32
nvswitch_get_sublink_width_ls10
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    NvU32 data = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLIPT,
                                     _NVLIPT_COMMON, _TOPOLOGY_LOCAL_LINK_CONFIGURATION);
    return DRF_VAL(_NVLIPT_COMMON, _TOPOLOGY_LOCAL_LINK_CONFIGURATION, _NUM_LANES_PER_LINK, data);
}

void
nvswitch_corelib_get_uphy_load_ls10
(
    nvlink_link *link,
    NvBool *bUnlocked
)
{
    *bUnlocked = NV_FALSE;
}

NvlStatus
nvswitch_corelib_set_dl_link_mode_ls10
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32            val;
    NvlStatus        status = NVL_SUCCESS;
    NvBool           keepPolling;
    NVSWITCH_TIMEOUT timeout;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return -NVL_UNBOUND_DEVICE;
    }

    switch (mode)
    {
        case NVLINK_LINKSTATE_INITPHASE1:
        {
            // Apply appropriate SIMMODE settings
            status = nvswitch_minion_set_sim_mode_ls10(device, link);
            if (status != NVL_SUCCESS)
            {
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            // Apply appropriate SMF settings
            status = nvswitch_minion_set_smf_settings_ls10(device, link);
            if (status != NVL_SUCCESS)
            {
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            // Apply appropriate UPHY Table settings
            status = nvswitch_minion_select_uphy_tables_ls10(device, link);
            if (status != NVL_SUCCESS)
            {
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            // Before INITPHASE1, apply NEA setting
            nvswitch_setup_link_loopback_mode(device, link->linkNumber);

            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITPHASE1 failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_CONFIGURATION_ERROR,
                    NVBIT64(link->linkNumber), INITPHASE1_ERROR);
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            break;
        }

        case NVLINK_LINKSTATE_POST_INITOPTIMIZE:
        {
            // Poll for TRAINING_GOOD
            status  = nvswitch_minion_get_initoptimize_status_ls10(device, link->linkNumber);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                            "%s Error polling for INITOPTIMIZE TRAINING_GOOD. Link (%s):(%s)\n",
                            __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT64(link->linkNumber), INITOPTIMIZE_ERROR);
                return NV_ERR_NVLINK_TRAINING_ERROR;
            }
            break;
        }

        case NVLINK_LINKSTATE_INITTL:
        {
             status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITTL, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITTL failed for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT64(link->linkNumber), INITTL_ERROR);
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }
            break;
        }
        case NVLINK_LINKSTATE_INITOPTIMIZE:
        {
            return nvswitch_corelib_set_dl_link_mode_lr10(link, mode, flags);
        }

        case NVLINK_LINKSTATE_INITPHASE5:
        {
            status = nvswitch_minion_send_command(device, link->linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE5A, 0);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s : INITPHASE5A failed to be called for link (%s):(%s).\n",
                    __FUNCTION__, device->name, link->linkName);
                NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT64(link->linkNumber), INITPHASE5_ERROR);
                return NV_ERR_NVLINK_CONFIGURATION_ERROR;
            }

            nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

            do
            {
                keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

                val =  NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLPHYCTL_COMMON, _PSAVE_UCODE_CTRL_STS);
                if(FLD_TEST_DRF(_NVLPHYCTL_COMMON, _PSAVE_UCODE_CTRL_STS, _PMSTS, _PSL0, val))
                {
                    break;
                }

                if(!keepPolling)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s : Failed to poll for L0 on link (%s):(%s).\n",
                        __FUNCTION__, device->name, link->linkName);
                    NVSWITCH_ASSERT_INFO(NV_ERR_NVLINK_TRAINING_ERROR, NVBIT64(link->linkNumber), INITPHASE5_ERROR);
                    return NV_ERR_NVLINK_CONFIGURATION_ERROR;

                }
            }
            while (keepPolling);

            break;
        }
        default:
        {
            status = nvswitch_corelib_set_dl_link_mode_lr10(link, mode, flags);
        }
    }

    return status;
}

NvlStatus
nvswitch_corelib_get_rx_detect_ls10
(
    nvlink_link *link
)
{
    NvlStatus status;
    nvswitch_device *device = link->dev->pDevInfo;

    status = nvswitch_minion_get_rxdet_status_ls10(device, link->linkNumber);

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
nvswitch_reset_persistent_link_hw_state_ls10
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    NvU32 clocksMask = NVSWITCH_PER_LINK_CLOCK_SET(RXCLK)|NVSWITCH_PER_LINK_CLOCK_SET(TXCLK)|
                            NVSWITCH_PER_LINK_CLOCK_SET(NCISOCCLK);
    nvlink_link *link = nvswitch_get_link(device, linkNumber);
    if ((link == NULL) || nvswitch_is_link_in_reset(device, link))
    {
        return;
    }

    // clear DL error counters
    (void)nvswitch_minion_send_command(device, linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_DLSTAT_CLR_DLERRCNT, 0);

    // If TLC is not up then return

    if (!nvswitch_are_link_clocks_on_ls10(device, link, clocksMask))
    {
        return;
    }

    // SETUPTC called to reset and setup throughput counters
    (void)nvswitch_minion_send_command(device, linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_SETUPTC , 0x4);

    // clear miscellaneous TLC counters and registers
    (void)nvswitch_minion_send_command(device, linkNumber, NV_MINION_NVLINK_DL_CMD_COMMAND_CLR_TLC_MISC_REGS, 0);

}

NvlStatus
nvswitch_corelib_get_tl_link_mode_ls10
(
    nvlink_link *link,
    NvU64 *mode
)
{
#if defined(INCLUDE_NVLINK_LIB)

    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 link_state;
    NvU32 val = 0;
    NvlStatus status = NVL_SUCCESS;

    *mode = NVLINK_LINKSTATE_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
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
    val = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
            _NVLIPT_LNK, _CTRL_LINK_STATE_STATUS);

    link_state = DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_STATUS, _CURRENTLINKSTATE,
            val);

    if (cciIsLinkManaged(device, link->linkNumber))
    {
        if (link_state == NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_RESET)
        {
            *mode = NVLINK_LINKSTATE_RESET;
            return NVL_SUCCESS;
        }
    }

    switch(link_state)
    {
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_ACTIVE:

            // If using ALI, ensure that the request to active completed
            if (link->dev->enableALI)
            {
                status = nvswitch_wait_for_tl_request_ready_ls10(link);
            }

            *mode = (status == NVL_SUCCESS) ? NVLINK_LINKSTATE_HS:NVLINK_LINKSTATE_OFF;
            break;

        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_L2:
            *mode = NVLINK_LINKSTATE_SLEEP;
            break;

        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_CONTAIN:
            *mode = NVLINK_LINKSTATE_CONTAIN;
            break;

        case NV_NVLIPT_LNK_CTRL_LINK_STATE_STATUS_CURRENTLINKSTATE_ACTIVE_PENDING:
            *mode = NVLINK_LINKSTATE_ACTIVE_PENDING;
            break;

        default:
            // Currently, only ACTIVE, L2 and CONTAIN states are supported
            return NVL_ERR_INVALID_STATE;
            break;
    }

#endif

    return status;
}

NvBool
nvswitch_is_link_in_reset_ls10
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    NvU32 clkStatus;
    NvU32 resetRequestStatus;

    clkStatus = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_CLK_CTRL);

    // Read the reset request register
    resetRequestStatus = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                         NVLIPT_LNK, _NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET);

    //
    // For link to be in reset either of 2 conditions should be true
    // 1. On a cold-boot the RESET_RSTSEQ status should be ASSERTED reset
    // 2. A link's current TL link state should be _RESET
    // and all of the per link clocks, RXCLK, TXCLK and NCISOCCLK, should be off
    //
    if ((DRF_VAL(_NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET, _LINK_RESET_STATUS, resetRequestStatus) ==
               NV_NVLIPT_LNK_RESET_RSTSEQ_LINK_RESET_LINK_RESET_STATUS_ASSERTED)     ||
        (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _RXCLK_STS, _OFF, clkStatus)      &&
        FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _TXCLK_STS, _OFF, clkStatus)       &&
        FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _NCISOCCLK_STS, _OFF, clkStatus)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

void
nvswitch_init_buffer_ready_ls10
(
    nvswitch_device *device,
    nvlink_link *link,
    NvBool bNportBufferReady
)
{
    NvU32 val;
    NvU32 linkNum = link->linkNumber;
    NvU64 forcedConfigLinkMask;
    NvU32 localLinkNumber = linkNum % NVSWITCH_LINKS_PER_MINION_LS10;
    NvU32 regData;
    regData = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
            NVLTLC, _NVLTLC_TX_SYS, _CTRL_BUFFER_READY);

    // If buffer ready is set then return
    if (FLD_TEST_DRF(_NVLTLC, _TX_SYS_CTRL_BUFFER_READY, _BUFFERRDY, _ENABLE, regData))
    {
        return;
    }

    forcedConfigLinkMask = ((NvU64)device->regkeys.chiplib_forced_config_link_mask) +
                ((NvU64)device->regkeys.chiplib_forced_config_link_mask2 << 32);

    //
    // Use legacy LS10 function to set buffer ready if
    // running with forced config since MINION is not
    // booted
    //
    if (forcedConfigLinkMask != 0)
    {
        nvswitch_init_buffer_ready_lr10(device, link, bNportBufferReady);
    }

    if (FLD_TEST_DRF(_SWITCH_REGKEY, _SKIP_BUFFER_READY, _TLC, _NO,
                     device->regkeys.skip_buffer_ready))
    {
        NVSWITCH_MINION_WR32_LS10(device,
                NVSWITCH_GET_LINK_ENG_INST(device, linkNum, MINION),
                _MINION, _NVLINK_DL_CMD_DATA(localLinkNumber),
                NV_MINION_NVLINK_DL_CMD_DATA_DATA_SET_BUFFER_READY_TX_AND_RX);

        nvswitch_minion_send_command(device, linkNum,
            NV_MINION_NVLINK_DL_CMD_COMMAND_SET_BUFFER_READY, 0);
    }

    if (bNportBufferReady &&
        FLD_TEST_DRF(_SWITCH_REGKEY, _SKIP_BUFFER_READY, _NPORT, _NO,
                     device->regkeys.skip_buffer_ready))
    {
        if (nvswitch_is_tnvl_mode_locked(device))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        }
        else
        {
            val = DRF_NUM(_NPORT, _CTRL_BUFFER_READY, _BUFFERRDY, 0x1);
            NVSWITCH_LINK_WR32_LS10(device, linkNum, NPORT, _NPORT, _CTRL_BUFFER_READY, val);
        }
    }
}

void
nvswitch_apply_recal_settings_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvU32 linkNumber = link->linkNumber;
    NvU32 regVal;
    NvU32 settingVal;

    // If no recal settings are set then return early
    if (device->regkeys.link_recal_settings == NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP)
    {
        return;
    }

    regVal = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
                _CTRL_SYSTEM_LINK_CHANNEL_CTRL2);

    settingVal = DRF_VAL(_SWITCH_REGKEY, _LINK_RECAL_SETTINGS, _MIN_RECAL_TIME_MANTISSA,
                    device->regkeys.link_recal_settings);
    if (settingVal != NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP)
    {
        regVal = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2,
                _L1_MINIMUM_RECALIBRATION_TIME_MANTISSA, settingVal, regVal);
    }

    settingVal = DRF_VAL(_SWITCH_REGKEY, _LINK_RECAL_SETTINGS, _MIN_RECAL_TIME_EXPONENT,
                    device->regkeys.link_recal_settings);
    if (settingVal != NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP)
    {
        regVal = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2,
                _L1_MINIMUM_RECALIBRATION_TIME_EXPONENT, 0x2, regVal);
    }

    settingVal = DRF_VAL(_SWITCH_REGKEY, _LINK_RECAL_SETTINGS, _MAX_RECAL_PERIOD_MANTISSA,
                    device->regkeys.link_recal_settings);
    if (settingVal != NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP)
    {
        regVal = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2,
                _L1_MAXIMUM_RECALIBRATION_PERIOD_MANTISSA, 0xf, regVal);
    }

    settingVal = DRF_VAL(_SWITCH_REGKEY, _LINK_RECAL_SETTINGS, _MAX_RECAL_PERIOD_EXPONENT,
                    device->regkeys.link_recal_settings);
    if (settingVal != NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP)
    {
        regVal = FLD_SET_DRF_NUM(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL2,
            _L1_MAXIMUM_RECALIBRATION_PERIOD_EXPONENT, 0x3, regVal);
    }

    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLIPT_LNK, _NVLIPT_LNK,
            _CTRL_SYSTEM_LINK_CHANNEL_CTRL2, regVal);

    return;
}

NvlStatus
nvswitch_corelib_get_dl_link_mode_ls10
(
    nvlink_link *link,
    NvU64 *mode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 link_state;
    NvU32 val = 0;
    NvU64 tlLinkMode;

    *mode = NVLINK_LINKSTATE_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
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

    val = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_STATE);

    link_state = DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, val);

    switch (link_state)
    {
        case NV_NVLDL_TOP_LINK_STATE_STATE_INIT:
            *mode = NVLINK_LINKSTATE_OFF;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_HWPCFG:
        case NV_NVLDL_TOP_LINK_STATE_STATE_HWCFG:
            *mode = NVLINK_LINKSTATE_DETECT;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_SWCFG:
            *mode = NVLINK_LINKSTATE_SAFE;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_ACTIVE:
            *mode = NVLINK_LINKSTATE_HS;
            break;
        case NV_NVLDL_TOP_LINK_STATE_STATE_SLEEP:
            if (device->hal.nvswitch_corelib_get_tl_link_mode(link, &tlLinkMode) != NVL_SUCCESS ||
                tlLinkMode == NVLINK_LINKSTATE_SLEEP)
            {
                *mode = NVLINK_LINKSTATE_SLEEP;
            }
            else
            {
                *mode = NVLINK_LINKSTATE_HS;
            }
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

NvlStatus
nvswitch_corelib_get_rx_mode_ls10
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 rx_sublink_state;
    NvU32 data = 0;
    NvU64 dlLinkMode;
    *mode = NVLINK_SUBLINK_STATE_RX_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
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

    data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_RX, _SLSM_STATUS_RX);

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
        case NV_NVLDL_RX_SLSM_STATUS_RX_PRIMARY_STATE_OFF:
            if (device->hal.nvswitch_corelib_get_dl_link_mode(link, &dlLinkMode) != NVL_SUCCESS ||
                dlLinkMode != NVLINK_LINKSTATE_HS)
            {
                *mode = NVLINK_SUBLINK_STATE_RX_OFF;
            }
            else
            {
                *mode = NVLINK_SUBLINK_STATE_RX_LOW_POWER;
            }
            break;

        default:
            *mode = NVLINK_SUBLINK_STATE_RX_OFF;
            break;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_corelib_get_tx_mode_ls10
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 tx_sublink_state;
    NvU64 dlLinkMode;
    NvU32 data = 0;

    *mode = NVLINK_SUBLINK_STATE_TX_OFF;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
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

    data = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);

    tx_sublink_state = DRF_VAL(_NVLDL_TX, _SLSM_STATUS_TX, _PRIMARY_STATE, data);

    // Return NVLINK_SUBLINK_SUBSTATE_TX_STABLE for sub-state
    *subMode = NVLINK_SUBLINK_SUBSTATE_TX_STABLE;

    switch (tx_sublink_state)
    {
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
            if (device->hal.nvswitch_corelib_get_dl_link_mode(link, &dlLinkMode) != NVL_SUCCESS ||
                dlLinkMode != NVLINK_LINKSTATE_HS)
            {
                *mode = NVLINK_SUBLINK_STATE_TX_OFF;
            }
            else
            {
                *mode = NVLINK_SUBLINK_STATE_TX_LOW_POWER;
            }
            break;

        default:
            *mode = NVLINK_SUBLINK_STATE_TX_OFF;
            break;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_launch_ALI_link_training_ls10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvBool           bSync
)
{
    NvlStatus status = NVL_SUCCESS;

    if ((link == NULL) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber) ||
        (link->linkNumber >= NVSWITCH_NVLINK_MAX_LINKS))
    {
        return -NVL_UNBOUND_DEVICE;
    }

    if (!nvswitch_is_link_in_reset(device, link))
    {
        return NVL_SUCCESS;
    }

    NVSWITCH_PRINT(device, INFO,
            "%s: ALI launching on link: 0x%x\n",
            __FUNCTION__, link->linkNumber);

    // Apply appropriate SIMMODE settings
    status = nvswitch_minion_set_sim_mode_ls10(device, link);
    if (status != NVL_SUCCESS)
    {
        return NV_ERR_NVLINK_CONFIGURATION_ERROR;
    }

    // Apply appropriate SMF settings
    status = nvswitch_minion_set_smf_settings_ls10(device, link);
    if (status != NVL_SUCCESS)
    {
        return NV_ERR_NVLINK_CONFIGURATION_ERROR;
    }

    // Apply appropriate UPHY Table settings
    status = nvswitch_minion_select_uphy_tables_ls10(device, link);
    if (status != NVL_SUCCESS)
    {
        return NV_ERR_NVLINK_CONFIGURATION_ERROR;
    }

    // Before INITPHASE1, apply NEA setting
    nvswitch_setup_link_loopback_mode(device, link->linkNumber);

    //
    // Request active, but don't block. FM will come back and check
    // active link status by blocking on this TLREQ's completion
    //
    status = nvswitch_request_tl_link_state_ls10(link,
            NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_ACTIVE,
            bSync);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: TL link state request to active for ALI failed for link: 0x%x\n",
            __FUNCTION__, link->linkNumber);
    }

    return status;
}

void
nvswitch_store_topology_information_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvU32            tempval;

    link->bInitnegotiateConfigGood = NV_TRUE;
    link->remoteSid = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_SID_HI);
    link->remoteSid = link->remoteSid << 32;
    link->remoteSid |= NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_SID_LO);

    tempval = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _TOPOLOGY_REMOTE_LINK_INFO);
    link->remoteLinkId = DRF_VAL(_NVLIPT_LNK, _TOPOLOGY_REMOTE_LINK_INFO, _LINK_NUMBER, tempval);

    link->localSid = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT,
                                            _NVLIPT_COMMON, _TOPOLOGY_LOCAL_CHIP_SID_HI);
    link->localSid = link->localSid << 32;
    link->localSid |= NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT,
                                             _NVLIPT_COMMON, _TOPOLOGY_LOCAL_CHIP_SID_LO);

    tempval = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
                                             _NVLIPT_LNK, _TOPOLOGY_REMOTE_CHIP_TYPE);

    // Update the remoteDeviceType with NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE values.
    switch(tempval)
    {
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV3P0AMP:
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV4P0HOP:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;
        break;
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV3P0LRK:
        case NV_NVLIPT_LNK_TOPOLOGY_REMOTE_CHIP_TYPE_TYPE_NV4P0LAG:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH;
        break;
        default:
            link->remoteDeviceType = NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE;
        break;
    }
}

void
nvswitch_get_error_rate_threshold_ls10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNumber = link->linkNumber;
    NvU32 crcRegVal;

    crcRegVal = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, 
                                         _NVLDL_RX, _ERROR_RATE_CTRL);

    link->errorThreshold.thresholdMan = DRF_VAL(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_MAN,
                                                crcRegVal);
    link->errorThreshold.thresholdExp = DRF_VAL(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_EXP,
                                                crcRegVal);
    link->errorThreshold.timescaleMan = DRF_VAL(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_MAN,
                                                crcRegVal);
    link->errorThreshold.timescaleExp = DRF_VAL(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_EXP,
                                                crcRegVal);
}

void
nvswitch_set_error_rate_threshold_ls10
(
    nvlink_link *link,
    NvBool bSetDefault
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNumber = link->linkNumber;
    NvU32 crcShortRegkeyVal = device->regkeys.crc_bit_error_rate_short;
    NvU32 crcRegVal;

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

    crcRegVal  = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, 
                                         _NVLDL_RX, _ERROR_RATE_CTRL);

    //
    // Case 1: When a Regkey is provided. We use it to calculate crcRegVal.
    //
    // Case 2: When the bSetDefault variable is set to NV_FALSE. This can happen 
    // when any client/application like NSCQ would provide specific values for 
    // the error threshold. In this case we use those values to calculate crcRegVal.
    //
    // Case 3: In all other cases, we want the default values to be used, which are
    // provided in Bug 3365481.
    // 
    if(crcShortRegkeyVal != NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_DEFAULT)
    {
        NvU32 shortRateMask;
        shortRateMask = DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN)     |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN) |
                            DRF_SHIFTMASK(NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP);
        
        crcRegVal  &= ~shortRateMask;
        crcRegVal  |= crcShortRegkeyVal;

        link->errorThreshold.bUserConfig = NV_FALSE;
        link->errorThreshold.bInterruptTrigerred = NV_FALSE;
    }
    else if (!bSetDefault)
    {
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_MAN,
                                     link->errorThreshold.thresholdMan,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_EXP,
                                     link->errorThreshold.thresholdExp,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_MAN,
                                     link->errorThreshold.timescaleMan,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_EXP,
                                     link->errorThreshold.timescaleExp,
                                     crcRegVal);        
    }
    else
    {
        //
        // Please refer to Bug 3365481 for details about the CRC_BIT_ERROR_RATE_SHORT 
        // default values used below.
        //
        link->errorThreshold.thresholdMan =
            NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN_DEFAULT;
        link->errorThreshold.thresholdExp =
            NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP_DEFAULT;
        link->errorThreshold.timescaleMan =
            NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN_DEFAULT;
        link->errorThreshold.timescaleExp =
            NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP_DEFAULT;
        link->errorThreshold.bUserConfig = NV_FALSE;
        link->errorThreshold.bInterruptTrigerred = NV_FALSE;

        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_MAN,
                                     link->errorThreshold.thresholdMan,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_THRESHOLD_EXP,
                                     link->errorThreshold.thresholdExp,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_MAN,
                                     link->errorThreshold.timescaleMan,
                                     crcRegVal);
        crcRegVal = FLD_SET_DRF_NUM(_NVLDL_RX, _ERROR_RATE_CTRL, _SHORT_TIMESCALE_EXP,
                                     link->errorThreshold.timescaleExp,
                                     crcRegVal);
    }

    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, 
                            _NVLDL_RX, _ERROR_RATE_CTRL, crcRegVal);
}

void
nvswitch_configure_error_rate_threshold_interrupt_ls10
(
    nvlink_link *link,
    NvBool bEnable
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNumber = link->linkNumber;
    NvU32 intrRegVal;
    link->errorThreshold.bInterruptEn = bEnable;

    intrRegVal = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, 
                                         _NVLDL_TOP, _INTR_NONSTALL_EN);

    if (bEnable)
    {
        link->errorThreshold.bInterruptTrigerred = NV_FALSE;
        intrRegVal = FLD_SET_DRF_NUM(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, 1, 
                                     intrRegVal);
    }
    else
    {
        intrRegVal = FLD_SET_DRF_NUM(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, 0, 
                                     intrRegVal);
    }

    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, 
                            _NVLDL_TOP, _INTR_NONSTALL_EN, intrRegVal);
}

void
nvswitch_init_dlpl_interrupts_ls10
(
    nvlink_link *link
)
{
    nvswitch_device *device            = link->dev->pDevInfo;
    NvU32            linkNumber        = link->linkNumber;

    // W1C any stale state.
    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR, 0xffffffff);
    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_SW2, 0xffffffff);

    // Set the interrupt bits
    nvswitch_set_dlpl_interrupts_ls10(link);

    // Setup error rate thresholds
    nvswitch_set_error_rate_threshold_ls10(link, NV_TRUE);
    nvswitch_configure_error_rate_threshold_interrupt_ls10(link, NV_TRUE);
}

void
nvswitch_set_dlpl_interrupts_ls10
(
    nvlink_link *link
)
{
    nvswitch_device *device            = link->dev->pDevInfo;
    NvU32            linkNumber        = link->linkNumber;
    // Stall tree routes to INTR_A which is connected to NVLIPT fatal tree

    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_STALL_EN,
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_REPLAY, _DISABLE)               |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_RECOVERY_SHORT, _DISABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_FAULT_UP, _ENABLE)           |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_FAULT_DOWN, _ENABLE)         |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_RAM, _ENABLE)             |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_INTERFACE, _ENABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _TX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_FAULT_DL_PROTOCOL, _ENABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_SHORT_ERROR_RATE, _DISABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_ILA_TRIGGER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _RX_CRC_COUNTER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _LTSSM_PROTOCOL, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_STALL_EN, _MINION_REQUEST, _DISABLE));

    // NONSTALL -> NONFATAL
    NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TOP, _INTR_NONSTALL_EN,
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_REPLAY, _DISABLE)               |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_RECOVERY_SHORT, _DISABLE)       |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _LTSSM_FAULT_UP, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_RAM, _DISABLE)            |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_INTERFACE, _DISABLE)      |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _TX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_FAULT_SUBLINK_CHANGE, _DISABLE) |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_FAULT_DL_PROTOCOL, _DISABLE)    |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_SHORT_ERROR_RATE, _DISABLE)     |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_ILA_TRIGGER, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _RX_CRC_COUNTER, _ENABLE)           |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _LTSSM_PROTOCOL, _DISABLE)          |
              DRF_DEF(_NVLDL_TOP, _INTR_NONSTALL_EN, _MINION_REQUEST, _DISABLE));
}

static NvU32
_nvswitch_get_nvlink_linerate_ls10
(
    nvswitch_device *device,
    NvU32            val
)
{
    NvU32  lineRate = 0;
    switch (val)
    {
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_100_00000G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_100_00000_GBPS;
            break;
        case NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_106_25000G:
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_106_25000_GBPS;
            break;
        default:
            NVSWITCH_PRINT(device, SETUP, "%s:ERROR LINE_RATE = 0x%x requested by regkey\n",
                       __FUNCTION__, lineRate);
            lineRate = NV_NVLIPT_LNK_CTRL_SYSTEM_LINK_CLK_CTRL_LINE_RATE_ILLEGAL_LINE_RATE;
    }
    return lineRate;
}

void
nvswitch_setup_link_system_registers_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{   
    NvU32 regval, fldval;
    NvU32 lineRate = 0;

    // LINE_RATE SYSTEM register
    if (device->regkeys.nvlink_speed_control != NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_DEFAULT)
    {
        regval   = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
                                           _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL);
        lineRate = _nvswitch_get_nvlink_linerate_ls10(device, device->regkeys.nvlink_speed_control);
        regval   = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL,
                                    _LINE_RATE, lineRate, regval);
        NVSWITCH_PRINT(device, SETUP, "%s: LINE_RATE = 0x%x requested by regkey\n",
                       __FUNCTION__, lineRate);
        NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLIPT_LNK,
                            _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CLK_CTRL, regval);
    }

    // TXTRAIN SYSTEM register
    regval = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
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

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _OPTIMIZATION_ALGORITHM,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_OPTIMIZATION_ALGORITHM_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: OPTIMIZATION_ALGORITHM = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_OPTIMIZATION_ALGORITHM, fldval, regval);
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

    fldval = DRF_VAL(_SWITCH_REGKEY, _TXTRAIN_CONTROL, _MINIMUM_TRAIN_TIME_MANTISSA,
                     device->regkeys.txtrain_control);
    if (fldval != NV_SWITCH_REGKEY_TXTRAIN_CONTROL_MINIMUM_TRAIN_TIME_MANTISSA_NOP)
    {
        NVSWITCH_PRINT(device, SETUP, "%s: MINIMUM_TRAIN_TIME_MANTISSA = 0x%x requested by regkey\n",
                       __FUNCTION__, fldval);
        regval = FLD_SET_DRF_NUM(_NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL,
                                 _TXTRAIN_MINIMUM_TRAIN_TIME_MANTISSA, fldval, regval);
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

    NVSWITCH_LINK_WR32_LS10(device, link->linkNumber, NVLIPT_LNK,
                            _NVLIPT_LNK_CTRL_SYSTEM_LINK, _CHANNEL_CTRL, regval);

    nvswitch_apply_recal_settings(device, link);

    return;
}

void
nvswitch_load_link_disable_settings_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{   
    NvU32 regVal;

    // Read state from NVLIPT HW
    regVal = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber, NVLIPT_LNK,
             _NVLIPT_LNK, _CTRL_LINK_STATE_STATUS);

    if (FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_LINK_STATE_STATUS, _CURRENTLINKSTATE, _DISABLE, regVal))
    {

        if(cciIsLinkManaged(device, link->linkNumber))
        {
            NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d is cci managed and should not be disabled.\n",
            __FUNCTION__, link->linkNumber);
            return;
        }
        
        // Set link to invalid and unregister from corelib
        device->link[link->linkNumber].valid = NV_FALSE;
        nvlink_lib_unregister_link(link);
        nvswitch_destroy_link(link);
    }

    return;
}

void
nvswitch_execute_unilateral_link_shutdown_ls10
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvlStatus status = NVL_SUCCESS;
    NvU32 retry_count = 3;
    NvU32 link_state_request;
    NvU32 link_state;
    NvU32 stat_data = 0;
    NvU32 link_intr_subcode = MINION_OK;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLDL, link->linkNumber))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link #%d invalid\n",
            __FUNCTION__, link->linkNumber);
        return;
    }

    do
    {
        //
        // Perform unilateral shutdown
        // This follows "Unilateral variant" from NVLink 4.x Shutdown
        //
        // Status is explicitly ignored here since we are required to soldier-on
        // in this scenario
        //
        status = nvswitch_request_tl_link_state_ls10(link,
                   NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_SHUTDOWN, NV_TRUE);

        if (status == NVL_SUCCESS)
        {
            return;
        }


        link_state_request = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                                NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_REQUEST);

        link_state = DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _STATUS, link_state_request);

        if (nvswitch_minion_get_dl_status(device, link->linkNumber,
                          NV_NVLSTAT_MN00, 0, &stat_data) == NVL_SUCCESS)
        {
            link_intr_subcode = DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_SUBCODE, stat_data);

            if ((link_state == NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_STATUS_MINION_REQUEST_FAIL) &&
                (link_intr_subcode == MINION_ALARM_BUSY))
            {
                NVSWITCH_PRINT(device, INFO,
                               "%s: Retrying shutdown due to Minion DLCMD Fault subcode = 0x%x\n",
                               __FUNCTION__, link_intr_subcode);
                //
                // We retry the shutdown sequence 3 times when we see a MINION_REQUEST_FAIL
                // or MINION_ALARM_BUSY
                //
                retry_count--;
            }
            else
            {
                break;
            }
        }
        else
        {
            // Querying MINION for link_intr_subcode failed so retry
            retry_count--;
        }


    } while (retry_count);

    NVSWITCH_PRINT(device, ERROR,
        "%s: NvLink Shutdown has failed for link %d\n",
        __FUNCTION__, link->linkNumber);

    return;
}

NvlStatus
nvswitch_reset_and_train_link_ls10
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    NvlStatus  status      = NVL_SUCCESS;
    NvU32      retry_count = 3;
    NvU32      link_state_request;
    NvU32      link_state;
    NvU32      stat_data;
    NvU32      link_intr_subcode = MINION_OK;

    nvswitch_execute_unilateral_link_shutdown_ls10(link);
    nvswitch_corelib_clear_link_state_ls10(link);

    // If link is CCI managed then return early
    if (cciIsLinkManaged(device, link->linkNumber))
    {
        return NVL_SUCCESS;
    }

    //
    // When a link faults there could be a race between the driver requesting
    // reset and MINION processing Emergency Shutdown. Minion will notify if
    // such a collision happens and will deny the reset request, so try the
    // request up to 3 times
    //
    do
    {
        status = nvswitch_request_tl_link_state_ls10(link,
                 NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET, NV_TRUE);

        if (status == NVL_SUCCESS)
        {
            break;
        }
        else
        {

            link_state_request = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                                    NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_REQUEST);

            link_state = DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _STATUS,
                                    link_state_request);

            if (nvswitch_minion_get_dl_status(device, link->linkNumber,
                              NV_NVLSTAT_MN00, 0, &stat_data) == NVL_SUCCESS)
            {
                link_intr_subcode = DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_SUBCODE, stat_data);

                if ((link_state == NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_STATUS_MINION_REQUEST_FAIL) &&
                (link_intr_subcode == MINION_ALARM_BUSY))
                {

                    status = nvswitch_request_tl_link_state_ls10(link,
                             NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET, NV_TRUE);

                    //
                    // We retry the shutdown sequence 3 times when we see a MINION_REQUEST_FAIL
                    // or MINION_ALARM_BUSY
                    //
                    retry_count--;
                }
                else
                {
                    break;
                }
            }
            else
            {
                // failed to query minion for the link_intr_subcode so retry
                retry_count--;
            }
        }
    } while(retry_count);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NvLink Reset has failed for link %d\n",
            __FUNCTION__, link->linkNumber);

        return status;
    }

    status = nvswitch_launch_ALI_link_training(device, link, NV_FALSE);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NvLink failed to request ACTIVE for link %d\n",
            __FUNCTION__, link->linkNumber);
        return status;
    }

    return NVL_SUCCESS;
}

NvBool
nvswitch_are_link_clocks_on_ls10
(
    nvswitch_device *device,
    nvlink_link *link,
    NvU32 clocksMask
)
{
    NvU32  clockStatus;
    NvU32  clk;
    NvBool bIsOff = NV_FALSE;

    clockStatus = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                    NVLIPT_LNK, _NVLIPT_LNK, _CTRL_CLK_CTRL);

    FOR_EACH_INDEX_IN_MASK(32, clk, clocksMask)
    {
        switch(clk)
        {
            case NVSWITCH_PER_LINK_CLOCK_RXCLK:
            {
                bIsOff = FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _RXCLK_STS, _OFF, clockStatus);
                break;
            }
            case NVSWITCH_PER_LINK_CLOCK_TXCLK:
            {
                bIsOff = FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _TXCLK_STS, _OFF, clockStatus);
                break;
            }
            case NVSWITCH_PER_LINK_CLOCK_NCISOCCLK:
            {
                bIsOff = FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CLK_CTRL, _NCISOCCLK_STS, _OFF, clockStatus);
                break;
            }
            default:
                return NV_FALSE;
        }

        if (bIsOff)
        {
            return NV_FALSE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_TRUE;
}

static
NvlStatus
_nvswitch_tl_request_get_timeout_value_ls10
(
    nvswitch_device *device,
    NvU32  tlLinkState,
    NvU32  *timeoutVal
)
{
    switch (tlLinkState)
    {
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_ACTIVE:
            *timeoutVal = NV_NVLINK_TLREQ_TIMEOUT_ACTIVE;
            break;
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET:
            *timeoutVal = NV_NVLINK_TLREQ_TIMEOUT_RESET;
            break;
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_SHUTDOWN:
            *timeoutVal = NV_NVLINK_TLREQ_TIMEOUT_SHUTDOWN;
            break;
        case NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_L2:
            *timeoutVal = NV_NVLINK_TLREQ_TIMEOUT_L2;
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: Invalid tlLinkState %d provided!\n",
                        __FUNCTION__, tlLinkState);
            return NVL_BAD_ARGS;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_request_tl_link_state_ls10
(
    nvlink_link *link,
    NvU32        tlLinkState,
    NvBool       bSync
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvlStatus status = NVL_SUCCESS;
    NvU32 linkStatus;
    NvU32 lnkErrStatus;
    NvU32 bit;
    NvU32            timeoutVal;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LS10(device, NVLIPT_LNK, link->linkNumber))
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

    // Clear any pending FAILEDMINIONREQUEST status that maybe populated as it is stale now
    bit = DRF_NUM(_NVLIPT_LNK, _ERR_STATUS_0, _FAILEDMINIONREQUEST, 1);
    lnkErrStatus = NVSWITCH_LINK_RD32(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0);
    if (nvswitch_test_flags(lnkErrStatus, bit))
    {
        NVSWITCH_LINK_WR32(device, link->linkNumber, NVLIPT_LNK, _NVLIPT_LNK, _ERR_STATUS_0,
                bit);
    }


    // Request state through CTRL_LINK_STATE_REQUEST
    NVSWITCH_LINK_WR32_LS10(device, link->linkNumber,
            NVLIPT_LNK, _NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST,
            DRF_NUM(_NVLIPT_LNK, _CTRL_LINK_STATE_REQUEST, _REQUEST, tlLinkState));

    if (bSync)
    {

        // setup timeouts for the TL request
        status = _nvswitch_tl_request_get_timeout_value_ls10(device, tlLinkState, &timeoutVal);
        if (status != NVL_SUCCESS)
        {
            return NVL_ERR_INVALID_STATE;
        }

        nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * timeoutVal, &timeout);
        status = NVL_MORE_PROCESSING_REQUIRED;

        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            // Check for state requested
            linkStatus  = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
                NVLIPT_LNK , _NVLIPT_LNK , _CTRL_LINK_STATE_STATUS);

            if (DRF_VAL(_NVLIPT_LNK, _CTRL_LINK_STATE_STATUS, _CURRENTLINKSTATE, linkStatus) ==
                        tlLinkState)
            {
                status = NVL_SUCCESS;
                break;
            }

            nvswitch_os_sleep(1);
        }
        while(keepPolling);

        // Do one final check if the polling loop didn't see the target linkState
        if (status == NVL_MORE_PROCESSING_REQUIRED)
        {
            // Check for state requested
            linkStatus  = NVSWITCH_LINK_RD32_LS10(device, link->linkNumber,
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
    }

    return status;
}

// Initialize optical links for pre-training
NvlStatus
nvswitch_cci_initialization_sequence_ls10
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    NvlStatus    status;
    nvlink_link link;
    nvlink_device dev;

    link.linkNumber = linkNumber;
    link.dev = &dev;
    link.dev->pDevInfo = device;

    // Perform INITPHASE1
    status = nvswitch_minion_send_command(device, linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1 failed on link %d.\n",
            __FUNCTION__, linkNumber);
        return status;
    }

    // SET RX detect
    status = nvswitch_minion_send_command(device, linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_TURING_RXDET, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
              "%s: Set RXDET failed for link %d.\n",
              __FUNCTION__, linkNumber);
        return status;
    }

    // Enable Common mode on Tx
    status = _nvswitch_init_dl_pll(&link);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to enable common mode for link %d.\n",
            __FUNCTION__, linkNumber);
        return status;
    }

    status = nvswitch_minion_send_command(device, linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE5A, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                 "%s : NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE5A failed on link %d.\n",
                __FUNCTION__, linkNumber);
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_deinitialization_sequence_ls10
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    NvlStatus    status;
    nvlink_link link;
    nvlink_device dev;

    link.linkNumber = linkNumber;
    link.dev = &dev;
    link.dev->pDevInfo = device;

    // Perform ABORTRXDET
    status = nvswitch_minion_send_command(device, linkNumber,
                        NV_MINION_NVLINK_DL_CMD_COMMAND_ABORTRXDET, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : NV_MINION_NVLINK_DL_CMD_COMMAND_ABORTRXDET failed on link %d.\n",
            __FUNCTION__, linkNumber);
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_cci_enable_iobist_ls10
(
    nvswitch_device *device,
    NvU32 linkNumber,
    NvBool bEnable
)
{
    NvU32 val;

    if (bEnable)
    {
        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _CFGCLKGATEEN, _ENABLE, val);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _PRBSALT, _PAM4, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4, _MASK_SKIP_OUT, _INIT, val);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4, _MASK_COM_OUT, _INIT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_2);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_2, _SKIP_SYMBOL_0, _SYMBOL, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_2,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_3);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_3, _SKIP_SYMBOL_1, _SYMBOL, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_3,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_0);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_0, _COM_SYMBOL_0, _SYMBOL, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_0,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_1);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_1, _COM_SYMBOL_1, _SYMBOL, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_1,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4, _SEND_DATA_OUT, _INIT, val);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4, _RESET_WORD_CNT_OUT, _COUNT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _SKIPCOMINSERTERGEN_4,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIGREG, _TX_BIST_EN_IN, _ENABLE, val);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIGREG, _DISABLE_WIRED_ENABLE_IN, _ENABLE, val);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIGREG, _IO_BIST_MODE_IN, _ENABLE, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG, val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _DPG_PRBSSEEDLD, _ENABLE, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG,  val);

        nvswitch_os_sleep(5);

        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _DPG_PRBSSEEDLD, _INIT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _STARTTEST, _ENABLE, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG,  val);
    }
    else
    {
        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIG, _STARTTEST, _INIT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIG,  val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIGREG, _DISABLE_WIRED_ENABLE_IN, _INIT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG, val);

        val = NVSWITCH_LINK_RD32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG);
        val = FLD_SET_DRF(_NVLDL_TXIOBIST, _CONFIGREG, _TX_BIST_EN_IN, _INIT, val);
        NVSWITCH_LINK_WR32_LS10(device, linkNumber, NVLDL, _NVLDL_TXIOBIST, _CONFIGREG, val);
    }

    return NVL_SUCCESS;
}

NvBool
nvswitch_does_link_need_termination_enabled_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    // Not defined for LS10
    return NV_FALSE;
}

NvlStatus
nvswitch_link_termination_setup_ls10
(
    nvswitch_device *device,
    nvlink_link* link
)
{
    // Not supported for LS10
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_link_l1_capability_ls10
(
    nvswitch_device *device,
    NvU32 linkNum,
    NvBool *isL1Capable
)
{
    NvU32 regData;
    NvBool bL1Capable;

    regData = NVSWITCH_LINK_RD32_LS10(device, linkNum, NVLIPT_LNK, _NVLIPT_LNK, _CTRL_SYSTEM_LINK_AN1_CTRL);
    bL1Capable = FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_SYSTEM_LINK_AN1_CTRL, _PWRM_L1_ENABLE, _ENABLE, regData);

    regData = NVSWITCH_LINK_RD32_LS10(device, linkNum, NVLIPT_LNK, _NVLIPT_LNK, _CTRL_CAP_LOCAL_LINK_AN1);
    bL1Capable &= FLD_TEST_DRF(_NVLIPT_LNK, _CTRL_CAP_LOCAL_LINK_AN1, _PWRM_L1_SUPPORT, _SUPPORTED, regData);

    *isL1Capable = bL1Capable;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_link_l1_threshold_ls10
(
    nvswitch_device *device,
    NvU32 linkNum,
    NvU32 *lpThreshold
)
{
    NvU32 regData; 

    regData = NVSWITCH_LINK_RD32_LS10(device, linkNum, NVLIPT_LNK, _NVLIPT_LNK, _PWRM_L1_ENTER_THRESHOLD);
    *lpThreshold = DRF_VAL(_NVLIPT, _LNK_PWRM_L1_ENTER_THRESHOLD, _THRESHOLD, regData);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_link_l1_threshold_ls10
(
    nvlink_link *link,
    NvU32 lpEntryThreshold
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    NvU32 linkNum = link->linkNumber;
    NvU32 tempRegVal, lpThreshold;
    NvU8  softwareDesired;
    NvU64 biosVersion;

    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_NO)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (nvswitch_lib_get_bios_version(device, &biosVersion) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN, "%s Get VBIOS version failed.\n",
                       __FUNCTION__);
        biosVersion = 0;
    }

    if (device->regkeys.lp_threshold != NV_SWITCH_REGKEY_SET_LP_THRESHOLD_DEFAULT)
    {
         lpThreshold = device->regkeys.lp_threshold;
    }
    else if ((lpEntryThreshold == NVSWITCH_SET_NVLINK_L1_THRESHOLD_DEFAULT) ||
             (lpEntryThreshold == NV_SWITCH_REGKEY_SET_LP_THRESHOLD_DEFAULT))
    {
        if (biosVersion >= BUG_3797211_LS10_VBIOS_VERSION)
        {
            //
            // Read the default L1 Threshold programmed by the
            // VBIOS (version 96.10.41.00.00 and above).
            //
            lpThreshold = NVSWITCH_LINK_RD32_LS10(device, linkNum, NVLIPT_LNK,
                                                  _NVLIPT_LNK, _SCRATCH_WARM);
        }
        else
        {
            lpThreshold = 1;
        }
    }
    else
    {
        lpThreshold = lpEntryThreshold;
    }

    tempRegVal = 0;
    tempRegVal = FLD_SET_DRF_NUM(_NVLIPT, _LNK_PWRM_L1_ENTER_THRESHOLD,
                                 _THRESHOLD, lpThreshold, tempRegVal);
    NVSWITCH_LINK_WR32_LS10(device, linkNum, NVLIPT_LNK, _NVLIPT_LNK,
                            _PWRM_L1_ENTER_THRESHOLD, tempRegVal);

    //LP Entry Enable
    softwareDesired = NV_NVLIPT_LNK_PWRM_CTRL_L1_SOFTWARE_DESIRED_L1;
    tempRegVal = NVSWITCH_LINK_RD32_LS10(device, linkNum, NVLIPT_LNK,
                                         _NVLIPT_LNK, _PWRM_CTRL);
    tempRegVal = FLD_SET_DRF_NUM(_NVLIPT, _LNK_PWRM_CTRL, _L1_SOFTWARE_DESIRED,
                                 softwareDesired, tempRegVal);
    NVSWITCH_LINK_WR32_LS10(device, linkNum, NVLIPT_LNK, _NVLIPT_LNK,
                            _PWRM_CTRL, tempRegVal);

    return NVL_SUCCESS;
}

