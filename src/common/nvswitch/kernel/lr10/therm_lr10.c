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


#include "export_nvswitch.h"
#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/therm_lr10.h"
#include "soe/soeiftherm.h"
#include "rmflcncmdif_nvswitch.h"
#include "soe/soe_nvswitch.h"

#include "nvswitch/lr10/dev_therm.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"

//
// Thermal functions
//

//
// Initialize thermal offsets for External Tdiode.
//

NvlStatus
nvswitch_init_thermal_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

     // Mark everything invalid
    chip_device->tdiode.method = NVSWITCH_THERM_METHOD_UNKNOWN;

    return NVL_SUCCESS;
}

static void
_nvswitch_read_max_tsense_temperature
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info,
    NvU32 channel
)
{
    NvU32  offset;
    NvU32  temperature;

    temperature = nvswitch_reg_read_32(device, NV_THERM_TSENSE_MAXIMUM_TEMPERATURE);
    temperature = DRF_VAL(_THERM_TSENSE, _MAXIMUM_TEMPERATURE, _MAXIMUM_TEMPERATURE, temperature);

    if (channel == NVSWITCH_THERM_CHANNEL_LR10_TSENSE_MAX)
    {
        offset = nvswitch_reg_read_32(device, NV_THERM_TSENSE_U2_A_0_BJT_0_TEMPERATURE_MODIFICATIONS);
        offset = DRF_VAL(_THERM_TSENSE, _U2_A_0_BJT_0_TEMPERATURE_MODIFICATIONS, _TEMPERATURE_OFFSET, offset);

        // Temperature of the sensor reported equals calculation of the max temperature reported
        // from the TSENSE HUB plus the temperature offset programmed by SW. This offset needs to
        // be substracted to get the actual temperature of the sensor.
        temperature -= offset;
    }

    info->temperature[channel] = NV_TSENSE_FXP_9_5_TO_24_8(temperature);
    info->status[channel] = NVL_SUCCESS;
}

static void
_nvswitch_read_external_tdiode_temperature
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info,
    NvU32 channel
)
{
}

NvlStatus
nvswitch_ctrl_therm_read_temperature_lr10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info
)
{
    NvU32 channel;

    if (!info->channelMask)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: No channel given in the input.\n",
            __FUNCTION__);

        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(info->temperature, 0x0, sizeof(info->temperature));

    channel = NVSWITCH_THERM_CHANNEL_LR10_TSENSE_MAX;
    if (info->channelMask & NVBIT(channel))
    {
        _nvswitch_read_max_tsense_temperature(device, info, channel);
        info->channelMask &= ~NVBIT(channel);
    }

    channel = NVSWITCH_THERM_CHANNEL_LR10_TSENSE_OFFSET_MAX;
    if (info->channelMask & NVBIT(channel))
    {
        _nvswitch_read_max_tsense_temperature(device, info, channel);        
        info->channelMask &= ~NVBIT(channel);
    }

    channel = NVSWITCH_THERM_CHANNEL_LR10_TDIODE;
    if (info->channelMask & NVBIT(channel))
    {
        _nvswitch_read_external_tdiode_temperature(device, info, channel);
        info->channelMask &= ~NVBIT(channel);
    }

    channel = NVSWITCH_THERM_CHANNEL_LR10_TDIODE_OFFSET;
    if (info->channelMask & NVBIT(channel))
    {
        _nvswitch_read_external_tdiode_temperature(device, info, channel);
        info->channelMask &= ~NVBIT(channel);
    }

    if (info->channelMask)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ChannelMask %x absent on LR10.\n",
            __FUNCTION__, info->channelMask);

        return -NVL_BAD_ARGS;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_therm_get_temperature_limit_lr10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS *info
)
{
    NvU32 threshold;
    NvU32 temperature;

    threshold = nvswitch_reg_read_32(device, NV_THERM_TSENSE_THRESHOLD_TEMPERATURES);

    switch (info->thermalEventId)
    {
        case NVSWITCH_CTRL_THERMAL_EVENT_ID_WARN:
        {
            // Get Slowdown temperature
            temperature = DRF_VAL(_THERM_TSENSE, _THRESHOLD_TEMPERATURES,
                                  _WARNING_TEMPERATURE, threshold);
            break;
        }
        case NVSWITCH_CTRL_THERMAL_EVENT_ID_OVERT:
        {
            // Get Shutdown temperature
            temperature = DRF_VAL(_THERM_TSENSE, _THRESHOLD_TEMPERATURES,
                                  _OVERTEMP_TEMPERATURE, threshold);
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "Invalid Thermal Event Id: 0x%x\n", info->thermalEventId);
            return -NVL_BAD_ARGS;
        }
    }
    
    info->temperatureLimit = NV_TSENSE_FXP_9_5_TO_24_8(temperature);

    return NVL_SUCCESS;
}

// Background task to monitor thermal warn and adjust link mode
void
nvswitch_monitor_thermal_alert_lr10
(
    nvswitch_device *device
)
{
    return;
}

/*
 * @brief Callback function to recieve thermal messages from SOE.
 */
void
nvswitch_therm_soe_callback_lr10
(
    nvswitch_device *device,
    RM_FLCN_MSG *pGenMsg,
    void *pParams,
    NvU32 seqDesc,
    NV_STATUS status
)
{
    RM_SOE_THERM_MSG_SLOWDOWN_STATUS slowdown_status;
    RM_SOE_THERM_MSG_SHUTDOWN_STATUS shutdown_status;
    RM_FLCN_MSG_SOE *pMsg = (RM_FLCN_MSG_SOE *)pGenMsg;
    NvU32 temperature;
    NvU32 threshold;

    switch (pMsg->msg.soeTherm.msgType)
    {
        case RM_SOE_THERM_MSG_ID_SLOWDOWN_STATUS:
        {
            slowdown_status = pMsg->msg.soeTherm.slowdown;
            if (slowdown_status.bSlowdown)
            {
                if (slowdown_status.source.bTsense) // TSENSE_THERM_ALERT
                {
                    temperature = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(slowdown_status.maxTemperature);
                    threshold   = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(slowdown_status.warnThreshold);

                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_START,
                        "NVSWITCH Temperature %dC | TSENSE WARN Threshold %dC\n",
                        temperature, threshold);

                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_START,
                        "Thermal Slowdown Engaged | Temp higher than WARN Threshold\n");
                }

                if (slowdown_status.source.bPmgr) // PMGR_THERM_ALERT
                {
                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_START,
                        "Thermal Slowdown Engaged | PMGR WARN Threshold reached\n");
                }
            }
            else // REVERT_SLOWDOWN
            {
                temperature = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(slowdown_status.maxTemperature);
                threshold   = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(slowdown_status.warnThreshold);

                NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_END,
                    "NVSWITCH Temperature %dC | TSENSE WARN Threshold %dC\n",
                    temperature, threshold);

                NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_END,
                    "Thermal slowdown Disengaged\n");
            }
            break;
        }

        case RM_SOE_THERM_MSG_ID_SHUTDOWN_STATUS:
        {
            shutdown_status = pMsg->msg.soeTherm.shutdown;
            if (shutdown_status.source.bTsense) // TSENSE_THERM_SHUTDOWN
            {
                temperature = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(shutdown_status.maxTemperature);
                threshold   = RM_SOE_NV_TEMP_TO_CELSIUS_TRUNCED(shutdown_status.overtThreshold);

                NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_SHUTDOWN,
                    "NVSWITCH Temperature %dC | OVERT Threshold %dC\n",
                    temperature, threshold);

                NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_SHUTDOWN,
                    "TSENSE OVERT Threshold reached. Shutting Down\n");
            }


            if (shutdown_status.source.bPmgr) // PMGR_THERM_SHUTDOWN
            {
                NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_THERMAL_EVENT_START,
                    "PMGR OVERT Threshold reached. Shutting Down\n");
            }
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "%s Unknown message Id\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
        }
    }
}

//
// nvswitch_therm_read_voltage
//
// Temperature and voltage are only available on SKUs which have thermal and
// voltage sensors.
//

NvlStatus
nvswitch_ctrl_therm_read_voltage_lr10
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_VOLTAGE_PARAMS *info
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_therm_read_power_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_POWER_PARAMS *info
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

