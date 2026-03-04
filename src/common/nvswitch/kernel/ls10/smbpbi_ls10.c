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

#include "common_nvswitch.h"

#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"
#include "lr10/smbpbi_lr10.h"
#include "nvVer.h"

NvlStatus
nvswitch_smbpbi_alloc_ls10
(
    nvswitch_device *device
)
{
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_smbpbi_post_init_hal_ls10
(
    nvswitch_device *device
)
{
    return NVL_SUCCESS;
}

void
nvswitch_smbpbi_destroy_hal_ls10
(
    nvswitch_device *device
)
{
}

NvlStatus
nvswitch_smbpbi_get_dem_num_messages_ls10
(
    nvswitch_device *device,
    NvU8            *pMsgCount
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_inforom_dem_load_ls10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_smbpbi_dem_load_ls10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_smbpbi_log_message_ls10
(
    nvswitch_device *device,
    NvU32           num,
    NvU32           msglen,
    NvU8            *osErrorString
)
{
    struct smbpbi                   *pSmbpbi = device->pSmbpbi;
    RM_FLCN_CMD_SOE                 cmd;
    RM_SOE_SMBPBI_CMD_LOG_MESSAGE   *pLogCmd = &cmd.cmd.smbpbiCmd.logMessage;
    NvU8                            offset;
    NvU8                            segSize;
    NVSWITCH_TIMEOUT                timeout;
    NvU32                           cmdSeqDesc;
    FLCN                            *pFlcn;
    NvlStatus                       status;

    if ((pSmbpbi == NULL) || (device->pSoe == NULL) ||
        (pSmbpbi->logMessageNesting++ != 0))
    {
        goto nvswitch_smbpbi_log_message_exit;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SMBPBI;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(SMBPBI, LOG_MESSAGE);
    cmd.cmd.smbpbiCmd.cmdType = RM_SOE_SMBPBI_CMD_ID_LOG_MESSAGE;

    msglen = NV_MIN(msglen, RM_SOE_SMBPBI_CMD_LOG_MESSAGE_MAX_STRING);

    pLogCmd->sxidId = num;
    pLogCmd->msgLen = msglen;
    pLogCmd->timeStamp = nvswitch_os_get_platform_time_epoch() / NVSWITCH_NSEC_PER_SEC;

    for (offset = 0; msglen > 0; offset += segSize)
    {
        segSize = NV_MIN(msglen, RM_SOE_SMBPBI_CMD_LOG_MESSAGE_STRING_SEGMENT_SZ);
        nvswitch_os_memcpy(pLogCmd->errorString, osErrorString + offset, segSize);

        pLogCmd->msgOffset = offset;
        pLogCmd->segSize = segSize;

        nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
        status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: SMBPBI Log Message command failed. rc:%d\n",
                            __FUNCTION__, status);
            break;
        }

        msglen -= segSize;
    }

    pSmbpbi->logMessageNesting = 0;

nvswitch_smbpbi_log_message_exit:
    return;
}

NvlStatus
nvswitch_smbpbi_send_init_data_ls10
(
    nvswitch_device *device
)
{
    struct smbpbi                   *pSmbpbi = device->pSmbpbi;
    RM_FLCN_CMD_SOE                 cmd;
    RM_SOE_SMBPBI_CMD_INIT_DATA     *pInitDataCmd = &cmd.cmd.smbpbiCmd.initData;
    NVSWITCH_TIMEOUT                timeout;
    NvU32                           cmdSeqDesc;
    FLCN                            *pFlcn = device->pSoe->pFlcn;
    NvlStatus                       status = NVL_SUCCESS;

    ct_assert(sizeof(NV_VERSION_STRING) <= sizeof(pInitDataCmd->driverVersionString));

    if (pSmbpbi == NULL)
    {
        goto nvswitch_smbpbi_send_init_data_exit;
    }

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SMBPBI;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(SMBPBI, INIT_DATA);
    cmd.cmd.smbpbiCmd.cmdType = RM_SOE_SMBPBI_CMD_ID_INIT_DATA;

    nvswitch_os_strncpy((char *)pInitDataCmd->driverVersionString,
                        NV_VERSION_STRING,
                        sizeof(pInitDataCmd->driverVersionString));
    pInitDataCmd->driverVersionString[sizeof(pInitDataCmd->driverVersionString) - 1] = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                    (PRM_FLCN_CMD)&cmd,
                    NULL,   // pMsg             - not used for now
                    NULL,   // pPayload         - not used for now
                    SOE_RM_CMDQ_LOG_ID,
                    &cmdSeqDesc,
                    &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: SMBPBI Init Data command failed. rc:%d\n",
                       __FUNCTION__, status);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "%s: SMBPBI Init Data sent to SOE.\n",
                       __FUNCTION__);
    }

nvswitch_smbpbi_send_init_data_exit:
    return status;
}

void
nvswitch_smbpbi_send_unload_ls10
(
    nvswitch_device *device
)
{
    nvswitch_smbpbi_send_unload_lr10(device);
}

void
nvswitch_smbpbi_dem_flush_ls10
(
    nvswitch_device *device
)
{
}
