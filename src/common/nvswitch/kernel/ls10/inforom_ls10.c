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
#include "ls10/ls10.h"
#include "ls10/inforom_ls10.h"
#include "inforom/ifrstruct.h"
#include "soe/soeififr.h"
#include "rmsoecmdif.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

NvlStatus
nvswitch_inforom_nvl_log_error_event_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pNvlErrorEvent,
    NvBool *bDirty
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

NvlStatus nvswitch_inforom_nvl_update_link_correctable_error_info_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pData,
    NvU8 linkId,
    NvU8 nvliptInstance,
    NvU8 localLinkIdx,
    void *pNvlErrorCounts,
    NvBool *bDirty
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

NvlStatus
nvswitch_oms_inforom_flush_ls10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

void
nvswitch_initialize_oms_state_ls10
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
)
{
    return;
}

NvBool
nvswitch_oms_get_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState
)
{
    return NV_FALSE;
}

void
nvswitch_oms_set_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bForceDeviceDisable
)
{
    return;
}

void
nvswitch_inforom_ecc_get_total_errors_ls10
(
    nvswitch_device    *device,
    INFOROM_ECC_OBJECT *pEccGeneric,
    NvU64              *pCorrectedTotal,
    NvU64              *pUncorrectedTotal
)
{
    return;
}

NvlStatus
nvswitch_inforom_load_obd_ls10
(
    nvswitch_device *device
)
{
    struct inforom *pInforom = device->pInforom;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return nvswitch_inforom_load_object(device, pInforom, "OBD",
                                        INFOROM_OBD_OBJECT_V2_XX_FMT,
                                        pInforom->OBD.packedObject.v2,
                                        &pInforom->OBD.object.v2);
}

NvlStatus
nvswitch_bbx_add_sxid_ls10
(
    nvswitch_device *device,
    NvU32            exceptionType,
    NvU32            data0,
    NvU32            data1,
    NvU32            data2
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc;
    NVSWITCH_TIMEOUT           timeout;

    if (!nvswitch_is_inforom_supported_ls10(device))
    {
        NVSWITCH_PRINT(device, INFO, "InfoROM is not supported, skipping\n");
        return NVL_SUCCESS;
    }

    // Avoid logging SOE related SXIDs to prevent recursive errors
    if (exceptionType > NVSWITCH_ERR_HW_SOE && exceptionType < NVSWITCH_ERR_HW_SOE_LAST)
    {
        NVSWITCH_PRINT(device, INFO, "Not logging SXID: %d to InfoROM\n", exceptionType);
        return NVL_SUCCESS;
    }

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_SXID_ADD;
    bbxCmd.cmd.ifr.bbxSxidAdd.exceptionType = exceptionType;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[0] = data0;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[1] = data1;
    bbxCmd.cmd.ifr.bbxSxidAdd.data[2] = data2;

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_unload_ls10
(
    nvswitch_device *device
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc;
    NVSWITCH_TIMEOUT           timeout;

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_SHUTDOWN;

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_load_ls10
(
    nvswitch_device *device,
    NvU64 time_ns,
    NvU8 osType,
    NvU32 osVersion
)
{
    NvlStatus                  status;
    FLCN                      *pFlcn;
    RM_FLCN_CMD_SOE            bbxCmd;
    NvU32                      cmdSeqDesc = 0;
    NVSWITCH_TIMEOUT           timeout;

    pFlcn = device->pSoe->pFlcn;
    nvswitch_timeout_create(NVSWITCH_INTERVAL_4SEC_IN_NS, &timeout);

    nvswitch_os_memset(&bbxCmd, 0, sizeof(bbxCmd));
    bbxCmd.hdr.unitId = RM_SOE_UNIT_IFR;
    bbxCmd.hdr.size = sizeof(bbxCmd);
    bbxCmd.cmd.ifr.cmdType = RM_SOE_IFR_BBX_INITIALIZE;
    bbxCmd.cmd.ifr.bbxInit.osType = osType;
    bbxCmd.cmd.ifr.bbxInit.osVersion = osVersion;
    RM_FLCN_U64_PACK(&bbxCmd.cmd.ifr.bbxInit.time, &time_ns);

    NVSWITCH_PRINT(device, INFO, "RM_SOE_IFR_BBX_INITIALIZE called, time_ns=%llu \n", time_ns);

    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&bbxCmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: BBX cmd %d failed. rc:%d\n",
                        __FUNCTION__, bbxCmd.cmd.ifr.cmdType, status);
    }

    return status;
}

NvlStatus
nvswitch_bbx_get_sxid_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}
