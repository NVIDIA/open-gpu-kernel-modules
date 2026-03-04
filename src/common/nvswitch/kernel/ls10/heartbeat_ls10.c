/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "error_nvswitch.h"
#include "export_nvswitch.h"
#include "ls10/soe_ls10.h"
#include "soe/soeifheartbeat.h"

/*!
 * @brief Callback function to recieve Heartbeat messages from SOE.
 */
void
nvswitch_heartbeat_soe_callback_ls10
(
    nvswitch_device *device,
    RM_FLCN_MSG     *pGenMsg,
    void            *pParams,
    NvU32            seqDesc,
    NV_STATUS        status
)
{
    RM_FLCN_MSG_SOE *pMsg = (RM_FLCN_MSG_SOE *)pGenMsg;

    switch (pMsg->msg.heartbeat.msgType)
    {
        case RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_WARN_ACTIVATED:
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_OSFP_THERM_WARN_ACTIVATED,
                "OSFP Thermal Warn Activated\n");
            break;
        }

        case RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_WARN_DEACTIVATED:
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_OSFP_THERM_WARN_DEACTIVATED,
                "OSFP Thermal Warn Deactivated\n");
            break;
        }

        case RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_OVERT_ACTIVATED:
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_OSFP_THERM_OVERT_ACTIVATED,
                "OSFP Thermal Overt Activated\n");
            break;
        }

        case RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_OVERT_DEACTIVATED:
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_OSFP_THERM_OVERT_DEACTIVATED,
                "OSFP Thermal Overt Deactivated\n");
            break;
        }
        case RM_SOE_HEARTBEAT_MSG_ID_OSFP_THERM_HEARTBEAT_SHUTDOWN:
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_OSFP_THERM_HEARTBEAT_SHUTDOWN,
                "OSFP Thermal SOE Heartbeat Shutdown\n");
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "%s Unknown message Id\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
        }
    }
}
