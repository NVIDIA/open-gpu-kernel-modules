/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "ls10/ls10.h"
#include "ls10/minion_ls10.h"
#include "ls10/minion_nvlink_defines_public_ls10.h"
#include "regkey_nvswitch.h"

#include "nvswitch/ls10/dev_minion_ip.h"
#include "nvswitch/ls10/dev_minion_ip_addendum.h"
#include "nvswitch/ls10/dev_ingress_ip.h"
#include "nvswitch/ls10/dev_egress_ip.h"
#include "nvswitch/ls10/dev_riscv_pri.h"
#include "nvswitch/ls10/dev_nvlphyctl_ip.h"

#include "flcn/flcn_nvswitch.h"

/*
 * @Brief : Check if MINION is already running.
 *
 * The function assumes that if one of MINIONs is running, all of them are
 * running. This approach needs to be fixed.
 *
 * TODO: Refactor minion code to check for each minion's status individually.
 *
 * @param[in] device Bootstrap MINIONs on this device
 */
static NvBool
_nvswitch_check_running_minions
(
    nvswitch_device *device
)
{
    NvU32  data, i;
    NvBool bMinionRunning = NV_FALSE;

    for (i = 0; i < NUM_MINION_ENGINE_LS10; i++)
    {
        if (!NVSWITCH_ENG_VALID_LS10(device, MINION, i))
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: MINION instance %d is not valid.\n",
                 __FUNCTION__, i);
            continue;
        }

        data = NVSWITCH_MINION_RD32_LS10(device, i, _CMINION, _FALCON_IRQSTAT);
        if (FLD_TEST_DRF(_CMINION, _FALCON_IRQSTAT, _HALT, _FALSE, data))
        {
            data = NVSWITCH_MINION_RD32_LS10(device, i, _MINION, _MINION_STATUS);
            if (FLD_TEST_DRF(_MINION,  _MINION_STATUS, _STATUS, _BOOT, data))
            {
                //
                // Set initialized flag if MINION is running.
                // We don't want to bootstrap a falcon that is already running.
                //
                nvswitch_set_minion_initialized(device, i, NV_TRUE);

                NVSWITCH_PRINT(device, SETUP,
                    "%s: MINION instance %d is already bootstrapped.\n",
                    __FUNCTION__, i);
                bMinionRunning = NV_TRUE;
            }
        }
    }

    return bMinionRunning;
}

NvlStatus
nvswitch_minion_get_dl_status_ls10
(
    nvswitch_device *device,
    NvU32            linkId,
    NvU32            statusIdx,
    NvU32            statusArgs,
    NvU32           *statusData
)
{
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32            regData, localLinkNumber;
    localLinkNumber = linkId % NVSWITCH_LINKS_PER_MINION_LS10;

    if (NVSWITCH_IS_LINK_ENG_VALID_LS10(device, MINION, linkId) &&
        !nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is not initialized for link %08x.\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
            linkId);
        return -NVL_ERR_INVALID_STATE;
    }

    // Query the DL status interface to get the data
    NVSWITCH_MINION_LINK_WR32_LS10(device, linkId, _MINION, _NVLINK_DL_STAT(localLinkNumber),
            DRF_NUM(_MINION, _NVLINK_DL_STAT, _ARGS, statusArgs) |
            DRF_NUM(_MINION, _NVLINK_DL_STAT, _STATUSIDX, statusIdx));

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        regData = NVSWITCH_MINION_LINK_RD32_LS10(device, linkId, _MINION, _NVLINK_DL_STAT(localLinkNumber));
        if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_STAT, _READY, 1, regData))
        {
            *statusData = NVSWITCH_MINION_LINK_RD32_LS10(device, linkId, _MINION, _NVLINK_DL_STATDATA(localLinkNumber));
            return NVL_SUCCESS;
        }
        if (IS_FMODEL(device) || IS_RTLSIM(device))
        {
            nvswitch_os_sleep(1);
        }
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for DL_STAT request to complete"
        " NV_MINION_NVLINK_DL_STAT(%d) = 0x%08x\n",
        __FUNCTION__, linkId, regData);
    return -NVL_ERR_INVALID_STATE;
}

/*
 * @Brief : Send MINION DL CMD for a particular link
 *
 * @param[in] device     Send command to MINION on this device
 * @param[in] linkNumber DLCMD will be sent on this link number
 *
 * @return           Returns true if the DLCMD passed
 */
NvlStatus
nvswitch_minion_send_command_ls10
(
    nvswitch_device *device,
    NvU32            linkNumber,
    NvU32            command,
    NvU32            scratch0
)
{
    NvU32            data = 0, localLinkNumber, statData = 0;
    NvU32            ingressEccRegVal = 0, egressEccRegVal = 0;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    localLinkNumber = linkNumber % NVSWITCH_LINKS_PER_MINION_LS10;

    if (NVSWITCH_IS_LINK_ENG_VALID_LS10(device, MINION, linkNumber) &&
        !nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is not initialized for link %08x.\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION),
            linkNumber);
        return NVL_SUCCESS;
    }

    data = NVSWITCH_MINION_LINK_RD32_LS10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber));
    if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, data))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is in fault state. NV_MINION_NVLINK_DL_CMD(%d) = %08x\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION),
            linkNumber, data);
        return -NVL_ERR_GENERIC;
    }

    // Write to minion scratch if needed by command
    switch (command)
    {
        case NV_MINION_NVLINK_DL_CMD_COMMAND_CONFIGEOM:
            data = 0;
            data = FLD_SET_DRF_NUM(_MINION, _MISC_0, _SCRATCH_SWRW_0, scratch0, data);
            NVSWITCH_MINION_WR32_LS10(device,
                NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION), _MINION, _MISC_0, data);
            break;
        case NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1:
            if (nvswitch_is_tnvl_mode_locked(device))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
                return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }

            //
            // WAR bug 2708497
            // Before INITPHASE1, we must clear these values, then set back to
            // _PROD after the call
            // NV_INGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
            // NV_EGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
            //

            ingressEccRegVal = NVSWITCH_NPORT_RD32_LS10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL);
            NVSWITCH_NPORT_WR32_LS10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL,
                FLD_SET_DRF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, _DISABLE, ingressEccRegVal));

            egressEccRegVal = NVSWITCH_NPORT_RD32_LS10(device, linkNumber, _EGRESS, _ERR_ECC_CTRL);
            NVSWITCH_NPORT_WR32_LS10(device, linkNumber, _EGRESS, _ERR_ECC_CTRL,
                FLD_SET_DRF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, _DISABLE, egressEccRegVal));
            break;
        default:
            break;
    }

    data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _COMMAND, command, data);
    data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT,   1,   data);
    NVSWITCH_MINION_LINK_WR32_LS10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber), data);

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    }

    //
    // We will exit this if the command is successful OR
    // if timeout waiting for the READY bit to be set OR
    // if it generates a MINION FAULT
    //
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        data = NVSWITCH_MINION_LINK_RD32_LS10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber));
        if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _READY, 1, data))
        {
            // The command has completed, success?
            if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, data))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: NVLink MINION command faulted!"
                    " NV_MINION_NVLINK_DL_CMD(%d) = 0x%08x\n",
                    __FUNCTION__, linkNumber, data);

                // Pull fault code and subcode
                if (nvswitch_minion_get_dl_status(device, linkNumber,
                            NV_NVLSTAT_MN00, 0, &statData) == NVL_SUCCESS)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: Minion DLCMD Fault code = 0x%x, Sub-code = 0x%x\n",
                        __FUNCTION__,
                        DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_CODE, statData),
                        DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_SUBCODE, statData));
                }
                else
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: Failed to get code and subcode from DLSTAT, link %d\n",
                        __FUNCTION__, linkNumber);
                }

                // Clear the fault and return
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Clearing NVLink MINION fault for link %d\n",
                    __FUNCTION__, linkNumber);

                data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, 0x0);
                NVSWITCH_MINION_LINK_WR32_LS10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber), data);
                return -NVL_ERR_INVALID_STATE;
            }
            else
            {
                NVSWITCH_PRINT(device, SETUP,
                    "%s: NVLink MINION command %x was sent successfully for link %d\n",
                    __FUNCTION__, command, linkNumber);
                break;
            }
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (!FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _READY, 1, data))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout waiting for NVLink MINION command to complete!"
            " NV_MINION_NVLINK_DL_CMD(%d) = 0x%08x\n",
            __FUNCTION__, linkNumber, data);
        return -NVL_ERR_INVALID_STATE;
    }

    if (command == NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1)
    {
        NVSWITCH_NPORT_WR32_LS10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL, ingressEccRegVal);
        NVSWITCH_NPORT_WR32_LS10(device, linkNumber, _EGRESS,  _ERR_ECC_CTRL, egressEccRegVal);
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Bootstrap all MINIONs on the specified device
 *
 * @param[in] device Bootstrap MINIONs on this device
 */
NvlStatus
nvswitch_init_minion_ls10
(
    nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;

    if (_nvswitch_check_running_minions(device))
    {
        return NVL_SUCCESS;
    }

    status = -NVL_INITIALIZATION_TOTAL_FAILURE;

    return status;
}

NvlStatus
nvswitch_set_minion_initialized_ls10
(
    nvswitch_device *device,
    NvU32 idx_minion,
    NvBool initialized
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    if (!NVSWITCH_ENG_VALID_LS10(device, MINION, idx_minion))
    {
        return -NVL_BAD_ARGS;
    }

    chip_device->engMINION[idx_minion].initialized = initialized;
    return NVL_SUCCESS;
}

NvBool
nvswitch_is_minion_initialized_ls10
(
    nvswitch_device *device,
    NvU32 idx_minion
)
{
    ls10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LS10(device);

    if (!NVSWITCH_ENG_VALID_LS10(device, MINION, idx_minion))
    {
        return NV_FALSE;
    }
    return (chip_device->engMINION[idx_minion].initialized != 0);
}

NvlStatus
nvswitch_minion_set_sim_mode_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvlStatus status = NVL_SUCCESS;
    NvU32 dlcmd;
    NvU32 linkNumber = link->linkNumber;
    NvU32 localLinkNumber = linkNumber % NVSWITCH_LINKS_PER_MINION_LS10;

    switch (device->regkeys.set_simmode)
    {
        case NV_SWITCH_REGKEY_MINION_SET_SIMMODE_FAST:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_FAST;
            break;
        case NV_SWITCH_REGKEY_MINION_SET_SIMMODE_MEDIUM:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_MEDIUM;
            break;
        case NV_SWITCH_REGKEY_MINION_SET_SIMMODE_SLOW:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_SLOW;
            break;
        default:
            return NVL_SUCCESS;
    }

    status = nvswitch_minion_send_command(device, linkNumber, dlcmd, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: DLCMD 0x%x failed on link: %d\n",
                __FUNCTION__, dlcmd, linkNumber);
        return status;
    }

    // Setting RXCAL_EN_ALARM timer value
    NVSWITCH_MINION_LINK_WR32_LS10(device, linkNumber, _MINION,
            _NVLINK_DL_CMD_DATA(localLinkNumber),
            NV_MINION_DL_CMD_DATA_RXCAL_EN_ALARM);

    status = nvswitch_minion_send_command(device, linkNumber,
        NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_RXCAL_EN_ALARM, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: DLCMD DBG_SETSIMMODE_RXCAL_EN_ALARM failed on link: %d\n",
                __FUNCTION__, linkNumber);
        return status;
    }

    // Setting INIT_CAL_DONE timer value
    NVSWITCH_MINION_LINK_WR32_LS10(device, linkNumber, _MINION,
            _NVLINK_DL_CMD_DATA(localLinkNumber),
            NV_MINION_DL_CMD_DATA_INIT_CAL_DONE);

    status = nvswitch_minion_send_command(device, linkNumber,
        NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_INIT_CAL_DONE, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: DLCMD DBG_SETSIMMODE_INIT_CAL_DONE failed on link: %d\n",
                __FUNCTION__, linkNumber);
        return status;
    }

    return status;
}

NvlStatus
nvswitch_minion_set_smf_settings_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvlStatus status = NVL_SUCCESS;
    NvU32 dlcmd;
    NvU32 linkNumber = link->linkNumber;

    switch (device->regkeys.set_smf_settings)
    {
        case NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_SLOW:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_SMF_VALUES_SLOW;
            break;
        case NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_MEDIUM:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_SMF_VALUES_MEDIUM;
            break;
        case NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_FAST:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_SMF_VALUES_FAST;
            break;
        case NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_MEDIUM_SERIAL:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_SMF_VALUES_MEDIUM_SERIAL;
            break;
        default:
            return NVL_SUCCESS;
    }

    status = nvswitch_minion_send_command(device, linkNumber, dlcmd, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: DLCMD 0x%x failed on link: %d\n",
                __FUNCTION__, dlcmd, linkNumber);
        return status;
    }

    return status;
}

NvlStatus
nvswitch_minion_select_uphy_tables_ls10
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    NvlStatus status = NVL_SUCCESS;
    NvU32 dlcmd;
    NvU32 linkNumber = link->linkNumber;

    switch (device->regkeys.select_uphy_tables)
    {
        case NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_SHORT:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_UPHY_TABLES_SHORT;
            break;
        case NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_FAST:
            dlcmd = NV_MINION_NVLINK_DL_CMD_COMMAND_DBG_SETSIMMODE_UPHY_TABLES_FAST;
            break;
        default:
            return NVL_SUCCESS;
    }

    status = nvswitch_minion_send_command(device, linkNumber, dlcmd, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: DLCMD 0x%x failed on link: %d\n",
                __FUNCTION__, dlcmd, linkNumber);
        return status;
    }

    return status;
}


NvlStatus
nvswitch_minion_get_rxdet_status_ls10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32            statData;
    NvlStatus        status;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(30*NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check RXDET status on MINION DL STAT interface
        status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_LNK2, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _LNK2, _RXDET_LINK_STATUS, _FOUND, statData))
        {
            NVSWITCH_PRINT(device, INFO,
                    "%s: RXDET LINK_STATUS = FOUND on link: %d\n",
                    __FUNCTION__, linkId);

            // Retrieve which lanes were found (should be all)
            device->link[linkId].lane_rxdet_status_mask =
                    DRF_VAL(_NVLSTAT, _LNK2, _RXDET_LANE_STATUS, statData);

            //
            // MINION doesn't have knowledge of lane reversal and therefore
            // reports logical lanes. We must reverse the bitmask here if applicable
            // since RM reports physical lanes.
            //
            if (nvswitch_link_lane_reversed_lr10(device, linkId))
            {
                NVSWITCH_REVERSE_BITMASK_32(NVSWITCH_NUM_LANES_LS10,
                        device->link[linkId].lane_rxdet_status_mask);
            }

            return NVL_SUCCESS;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _LNK2, _RXDET_LINK_STATUS, _TIMEOUT, statData))
        {
            NVSWITCH_PRINT(device, ERROR,
                    "%s: RXDET LINK_STATUS = TIMEOUT on link: %d\n",
                    __FUNCTION__, linkId);

            // Retrieve which lanes were found
            device->link[linkId].lane_rxdet_status_mask =
                    DRF_VAL(_NVLSTAT, _LNK2, _RXDET_LANE_STATUS, statData);

            //
            // MINION doesn't have knowledge of lane reversal and therefore
            // reports logical lanes. We must reverse the bitmask here if applicable
            // since RM reports physical lanes.
            //
            if (nvswitch_link_lane_reversed_lr10(device, linkId))
            {
                NVSWITCH_REVERSE_BITMASK_32(NVSWITCH_NUM_LANES_LS10,
                        device->link[linkId].lane_rxdet_status_mask);
            }

            return -NVL_ERR_INVALID_STATE;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for RXDET STATUS on link: %d\n",
        __FUNCTION__, linkId);

    return -NVL_ERR_INVALID_STATE;
}

/*
 * @Brief : Check if the RISCV CPU has started
 *
 * @param[in] device      The Nvswitch device
 * @param[in] idx_minion  MINION instance to use
 */
NvBool
nvswitch_minion_is_riscv_active_ls10
(
    nvswitch_device *device,
    NvU32           idx_minion
)
{
    NvU32 val;

    val = NVSWITCH_MINION_RD32_LS10(device, idx_minion, _CMINION_RISCV, _CPUCTL);

    return FLD_TEST_DRF(_CMINION, _RISCV_CPUCTL, _ACTIVE_STAT, _ACTIVE, val);

}

NvlStatus
nvswitch_minion_clear_dl_error_counters_ls10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvlStatus status;

    status = nvswitch_minion_send_command(device, linkId,
                                               NV_MINION_NVLINK_DL_CMD_COMMAND_DLSTAT_CLR_DLERRCNT, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s : Failed to clear error count to MINION for link # %d\n",
            __FUNCTION__, linkId);
    }
    return status;
}

NvlStatus
nvswitch_minion_send_inband_data_ls10
(
    nvswitch_device *device,
    NvU32            linkId,
    nvswitch_inband_send_data *inBandData
)
{
    NvlStatus status = NVL_SUCCESS;
#if defined(INCLUDE_NVLINK_LIB)
    NvlStatus tempStatus = NVL_SUCCESS;
    NvU32 localLinkNumber = linkId % NVSWITCH_LINKS_PER_MINION_LS10;
    NvU8  *sendBuffer = inBandData->sendBuffer;
    NvU32 bufferSize = inBandData->bufferSize;
    NvU32 data = 0;
    NvU32 regval = 0;
    NvU32 statData = 0;
    NVSWITCH_TIMEOUT timeout;
    NvBool bKeepPolling = NV_TRUE;

    if (bufferSize == 0 || bufferSize > NVLINK_INBAND_MAX_XFER_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR, "Bad Inband data size %d. Skipping Inband Send\n", bufferSize);
        return -NVL_ERR_INVALID_STATE;
    }

    // Buffer Size must be reduced by 1 as per the minion protocol
    regval = DRF_NUM(_MINION, _INBAND_SEND_DATA, _BUFFER_SIZE, (bufferSize - 1));

    regval |= DRF_NUM(_MINION, _INBAND_SEND_DATA, _FLAGS,inBandData->hdr.data);

    NVSWITCH_MINION_WR32_LS10(device,
                              NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
                              _MINION, _NVLINK_DL_CMD_DATA(localLinkNumber),
                              regval);

    status = nvswitch_minion_send_command(device, linkId,
                                          NV_MINION_NVLINK_DL_CMD_COMMAND_WRITE_TX_BUFFER_START, 0);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer for TX_BUFFER_START failed\n", linkId);
        return status;
    }

    while (bufferSize != 0)
    {
        nvswitch_os_memcpy(&data, sendBuffer, NV_MIN(sizeof(data), bufferSize));

        NVSWITCH_MINION_WR32_LS10(device, 
                                  NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
                                  _MINION, _NVLINK_DL_CMD_DATA(localLinkNumber),
                                  data);

        status = nvswitch_minion_send_command(device, linkId,
                                              NV_MINION_NVLINK_DL_CMD_COMMAND_WRITE_TX_BUFFER_MIDDLE, 0);

        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer failed\n", linkId);
            goto clear_buffer;
        }

        bufferSize -= NV_MIN(sizeof(data), bufferSize);
        sendBuffer += NV_MIN(sizeof(data), bufferSize);
    }

    status = nvswitch_minion_send_command(device, linkId,
                                          NV_MINION_NVLINK_DL_CMD_COMMAND_WRITE_TX_BUFFER_END, 0);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer for TX_BUFFER_END\n", linkId);
        goto clear_buffer;
    }

    // Wait for buffer complete or buffer fail
    nvswitch_timeout_create(2 * NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);
    do
    {
        bKeepPolling = !nvswitch_timeout_check(&timeout);
        // DLSTAT need to explicitly checked
        status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_UC01, 0, &statData);

        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, INFO,"%s : Failed to poll DLSTAT register for (%s):(%d)\n",
                            __FUNCTION__, device->name, linkId);
            status = -NVL_ERR_INVALID_STATE;
            goto clear_buffer;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _UC01, _INBAND_BUFFER_COMPLETE, _TRUE, statData))
        {
            return NVL_SUCCESS;
        }

        if  (FLD_TEST_DRF(_NVLSTAT, _UC01, _INBAND_BUFFER_FAIL, _TRUE, statData))
        {
            NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer BUFFER_FAILED\n", linkId);
            status = -NVL_ERR_INVALID_STATE;
            goto clear_buffer;
        }
        //
        // Consider a case where both FM and RM trying to write on the same NVLink at the same time.
        // Both are waiting on each other to read the buffer, but they have also blocked the ISR,
        // as while waiting they hold the device lock. Thus, it is necessary to unblock one of them.
        // And to do that we have service BUFFER_AVAILABLE while waiting.
        //
        nvswitch_service_minion_all_links_ls10(device);

        nvswitch_os_sleep(10);

    } while (bKeepPolling);

    NVSWITCH_PRINT(device, ERROR, "Link %d Inband Neither got BUFFER_FAIL nor BUFFER_COMPLETE\n", linkId);

clear_buffer:
    tempStatus = nvswitch_minion_send_command(device, linkId,
                                              NV_MINION_NVLINK_DL_CMD_COMMAND_CLEAR_TX_BUFFER,
                                              0);
    if (tempStatus != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer for TX_BUFFER_CLEAR\n", linkId);
        return status;
    }

    // Check if we received BUFFER_COMPLETE is seen while doing a BUFFER_CLEAR
    tempStatus = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_UC01, 0, &statData);

    if (tempStatus != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,"%s : Failed to poll DLSTAT register for (%s):(%d)\n",
                        __FUNCTION__, device->name, linkId);
        return status;
    }

    if (FLD_TEST_DRF(_NVLSTAT, _UC01, _INBAND_BUFFER_COMPLETE, _TRUE, statData))
    {
        status = NVL_SUCCESS;
    }
    else
    {
        status = bKeepPolling ? status: -NVL_ERR_STATE_IN_USE;
    }
#endif
    return status;
}

void
nvswitch_minion_receive_inband_data_ls10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvlStatus status = NVL_SUCCESS;
#if defined(INCLUDE_NVLINK_LIB)
    NvlStatus tempStatus = NVL_SUCCESS;
    NvU32 numEntries = 0;
    NvU32 i;
    NvU32 localLinkNumber = linkId % NVSWITCH_LINKS_PER_MINION_LS10;
    NvU8  *receiveBuffer;
    NvU32 regVal, dataSize, remainingBuffer, bytesToXfer;
    nvlink_inband_drv_hdr_t hdr;

    status = nvswitch_minion_send_command(device, linkId,
                                          NV_MINION_NVLINK_DL_CMD_COMMAND_READ_RX_BUFFER_START,
                                          0);
    if (status != NV_OK)
        goto cleanup;

    regVal  = NVSWITCH_MINION_RD32_LS10(device, 
                                        NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
                                        _MINION, _NVLINK_DL_CMD_DATA(localLinkNumber));

    // Add 1 to the data as per minion protocol
    dataSize   = DRF_VAL(_MINION, _INBAND_SEND_DATA, _BUFFER_SIZE, regVal) + 1;
    hdr.data   = DRF_VAL(_MINION, _INBAND_SEND_DATA, _FLAGS, regVal);
    numEntries = NV_ALIGN_UP(dataSize, NVLINK_INBAND_MAX_XFER_AT_ONCE)/
                                                 NVLINK_INBAND_MAX_XFER_AT_ONCE;
    remainingBuffer = dataSize;

    if ((hdr.data & (NVLINK_INBAND_DRV_HDR_TYPE_START |
                     NVLINK_INBAND_DRV_HDR_TYPE_MID   |
                     NVLINK_INBAND_DRV_HDR_TYPE_END)) == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "InBand: HDR is wrong\n");
        goto cleanup;
    }

    if (hdr.data & NVLINK_INBAND_DRV_HDR_TYPE_START)
    {
         if (device->link[linkId].inbandData.message != NULL)
         {
             NVSWITCH_PRINT(device, ERROR, "InBand: Got TYPE_START for existing data\n");
             NVSWITCH_ASSERT(0);
             goto cleanup;
         }

         device->link[linkId].inbandData.message =
                                   nvswitch_os_malloc(sizeof(nvswitch_inband_data_list));
         if (device->link[linkId].inbandData.message == NULL)
         {
             status = -NVL_NO_MEM;
             goto cleanup;
         }

         device->link[linkId].inbandData.message->dataSize = 0;
    }

    if (device->link[linkId].inbandData.message == NULL)
    {
         NVSWITCH_PRINT(device, ERROR, "InBand: Data being sent without _START\n");
         goto cleanup;
    }

    receiveBuffer = device->link[linkId].inbandData.message->data;

    receiveBuffer += device->link[linkId].inbandData.message->dataSize;

    if (((dataSize + device->link[linkId].inbandData.message->dataSize) >
                                            NVSWITCH_INBAND_DATA_SIZE) ||
         (dataSize > NVLINK_INBAND_MAX_XFER_SIZE) ||
         (dataSize == 0))
    {
         NVSWITCH_PRINT(device, ERROR, "InBand: Msg is of wrong Size :DataSize = %d Msg Size= %d\n",
                        dataSize, device->link[linkId].inbandData.message->dataSize);
         NVSWITCH_ASSERT(0);
         goto cleanup;
    }

    for (i = 0; i < numEntries; i++)
    {
        status = nvswitch_minion_send_command(device, linkId,
                                 NV_MINION_NVLINK_DL_CMD_COMMAND_READ_RX_BUFFER_MIDDLE, 0);
        if (status != NV_OK)
        {
             NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer receive" 
                                            "for entry %d failed\n", linkId, i);
             goto cleanup;
        }

        regVal = NVSWITCH_MINION_RD32_LS10(device, 
                                           NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
                                           _MINION, _NVLINK_DL_CMD_DATA(localLinkNumber));

        bytesToXfer = NV_MIN(remainingBuffer, NVLINK_INBAND_MAX_XFER_AT_ONCE);

        nvswitch_os_memcpy(receiveBuffer, &regVal, bytesToXfer);

        receiveBuffer += bytesToXfer;
        remainingBuffer -= bytesToXfer;
    }

    status = nvswitch_minion_send_command(device, linkId,
                                          NV_MINION_NVLINK_DL_CMD_COMMAND_READ_RX_BUFFER_END, 0);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer receive"
                                           "for entry %d failed\n", linkId, numEntries);
        goto cleanup;
    }

    device->link[linkId].inbandData.message->dataSize += dataSize;

    if (hdr.data & NVLINK_INBAND_DRV_HDR_TYPE_END)
    {
         nvswitch_filter_messages(device, linkId);
    }

    return;

cleanup:
    tempStatus = nvswitch_minion_send_command(device, linkId,
                                          NV_MINION_NVLINK_DL_CMD_COMMAND_CLEAR_RX_BUFFER,
                                          0);
    if (tempStatus != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Link %d Inband Buffer transfer for RX_BUFFER_CLEAR\n", linkId);
        return;
    }
    if (device->link[linkId].inbandData.message != NULL)
    {
        nvswitch_os_free(device->link[linkId].inbandData.message);
        device->link[linkId].inbandData.message = NULL;
    }
    //TODO: Check if we need to send a failure msg to client?
#endif
}

NvlStatus
nvswitch_minion_get_ali_debug_registers_ls10
(
    nvswitch_device *device,
    nvlink_link *link,
    NVSWITCH_MINION_ALI_DEBUG_REGISTERS *params
)
{
    NvU32 localLinkNumber = link->linkNumber % NVSWITCH_LINKS_PER_MINION_LS10;
    if (nvswitch_minion_get_dl_status(device, link->linkNumber,
             NV_NVLSTAT_MN00, 0, &(params->dlstatMn00)) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,"%s : Failed to poll DLSTAT _MN00 register for (%s):(%d)\n",
                        __FUNCTION__, device->name, link->linkNumber);
    }

    if (nvswitch_minion_get_dl_status(device, link->linkNumber,
             NV_NVLSTAT_UC01, 0, &(params->dlstatUc01)) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,"%s : Failed to poll DLSTAT UC01 register for (%s):(%d)\n",
                        __FUNCTION__, device->name, link->linkNumber);
    }

    params->dlstatLinkIntr = NVSWITCH_MINION_LINK_RD32_LS10(device, link->linkNumber,
                    _MINION, _NVLINK_LINK_INTR(localLinkNumber));

    return NVL_SUCCESS;
}

