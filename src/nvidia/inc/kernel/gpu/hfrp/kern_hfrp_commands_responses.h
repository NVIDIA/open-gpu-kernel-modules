/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



#ifndef _KERN_HFRP_COMMANDS_RESPONSES_H_
#define _KERN_HFRP_COMMANDS_RESPONSES_H_

#include "nvtypes.h"

// default parameter values
#define HFRP_DEFAULT_CLIENT_VERSION                             0U
#define HFRP_DEFAULT_SERVER_VERSION                             0U





/*!
 * CMD_SOC_SET_DEVICE_POWER_STATE
 *
 * This command sets device power state for Nvidia IPs.
 * It is expected that HFRP will follow device power state handling sequence specific to each device.
 *
 * Command Params:
 *  deviceId
 *        Specifies the device ID whose power state needs to be changed.
 *        0 - iGPU (This includes iGPU and Display)
 *        1 - DLA
 *        2 - HDA
 *  powerState
 *        0 - D0 i.e. Power up
 *        1 - D3 i.e. Power down
 *
 */
#define HFRP_CMD_SOC_SET_DEVICE_POWER_STATE                303U

#pragma pack(1)
typedef struct
{
    NvU8 deviceId;
    NvU8 powerState;
} CMD_SOC_SET_DEVICE_POWER_STATE_PARAMS;
#pragma pack()

#define NV_CMD_SOC_SET_DEVICE_POWER_STATE_PARAM_DEVICE_ID_IGPU         0U
#define NV_CMD_SOC_SET_DEVICE_POWER_STATE_PARAM_DEVICE_ID_DLA          1U
#define NV_CMD_SOC_SET_DEVICE_POWER_STATE_PARAM_DEVICE_ID_HDA          2U

#define NV_CMD_SOC_SET_DEVICE_POWER_STATE_PARAM_POWER_STATE_D0         0U
#define NV_CMD_SOC_SET_DEVICE_POWER_STATE_PARAM_POWER_STATE_D3         1U

#endif      // _KERN_HFRP_COMMANDS_RESPONSES_H_
