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

#include "nvfixedtypes.h"
#include "common_nvswitch.h"
#include "error_nvswitch.h"
#include "rmsoecmdif.h"
#include "smbpbi_nvswitch.h"
#include "nvswitch/lr10/dev_ext_devices.h"

#include "flcn/flcn_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"

NvlStatus
nvswitch_smbpbi_init
(
    nvswitch_device *device
)
{
    NV_STATUS                  status;

    if (!device->pSoe)
    {
        return -NVL_ERR_INVALID_STATE;
    }

    device->pSmbpbi = nvswitch_os_malloc(sizeof(struct smbpbi));
    if (!device->pSmbpbi)
    {
        status = -NVL_NO_MEM;
        goto smbpbi_init_fail;
    }
    nvswitch_os_memset(device->pSmbpbi, 0, sizeof(struct smbpbi));

    status = device->hal.nvswitch_smbpbi_send_init_data(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to send SMBPBI init data, rc:%d\n",
            status);
        // Do not fail the init because of this.
    }

    status = device->hal.nvswitch_smbpbi_alloc(device);

smbpbi_init_fail:
    if (status != NVL_SUCCESS)
    {
        nvswitch_os_free(device->pSmbpbi);
    }

    return status;
}

NvlStatus
nvswitch_smbpbi_post_init
(
    nvswitch_device * device
)
{
    NvlStatus   status;

    if (!device->pSmbpbi)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }
    status = device->hal.nvswitch_smbpbi_post_init_hal(device);

    if (status == NVL_SUCCESS)
    {
#if defined(DEBUG) || defined(DEVELOP) || defined(NV_MODS)
        nvswitch_lib_smbpbi_log_sxid(device, NVSWITCH_ERR_NO_ERROR,
                                     "NVSWITCH SMBPBI server is online.");
#endif // defined(DEBUG) || defined(DEVELOP) || defined(NV_MODS)

        NVSWITCH_PRINT(device, INFO, "%s: SMBPBI POST INIT completed\n", __FUNCTION__);
    }

    return status;
}

void
nvswitch_smbpbi_unload
(
    nvswitch_device *device
)
{
    if (device->pSmbpbi)
    {
        device->hal.nvswitch_smbpbi_send_unload(device);
        device->hal.nvswitch_smbpbi_dem_flush(device);
    }
}

void
nvswitch_smbpbi_destroy
(
    nvswitch_device *device
)
{
    if (device->pSmbpbi)
    {
        device->hal.nvswitch_smbpbi_destroy_hal(device);
        nvswitch_os_free(device->pSmbpbi);
        device->pSmbpbi = NULL;
    }
}

NvlStatus
nvswitch_smbpbi_refresh_ecc_counts
(
    nvswitch_device *device
)
{
    PRM_SOE_SMBPBI_INFOROM_DATA pObjs;
    struct inforom              *pInforom = device->pInforom;
    NvU64                       corCnt;
    NvU64                       uncCnt;

    if ((device->pSmbpbi == NULL) || (device->pSmbpbi->sharedSurface == NULL))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (pInforom == NULL || pInforom->pEccState == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    device->hal.nvswitch_inforom_ecc_get_total_errors(device, pInforom->pEccState->pEcc,
                                                      &corCnt, &uncCnt);

    pObjs = &device->pSmbpbi->sharedSurface->inforomObjects;
    NvU64_ALIGN32_PACK(&(pObjs->ECC.correctedTotal), &corCnt);
    NvU64_ALIGN32_PACK(&(pObjs->ECC.uncorrectedTotal), &uncCnt);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_smbpbi_set_link_error_info
(
    nvswitch_device *device,
    NVSWITCH_LINK_TRAINING_ERROR_INFO *pLinkTrainingErrorInfo,
    NVSWITCH_LINK_RUNTIME_ERROR_INFO  *pLinkRuntimeErrorInfo
)
{
    FLCN                                   *pFlcn;
    RM_FLCN_CMD_SOE                        cmd;
    NVSWITCH_TIMEOUT                       timeout;
    NvU32                                  cmdSeqDesc;
    RM_SOE_SMBPBI_CMD_SET_LINK_ERROR_INFO *pSetCmd = &cmd.cmd.smbpbiCmd.linkErrorInfo;
    NvlStatus                              status;

    if (!device->pSmbpbi)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SMBPBI;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(SMBPBI, SET_LINK_ERROR_INFO);
    cmd.cmd.smbpbiCmd.cmdType = RM_SOE_SMBPBI_CMD_ID_SET_LINK_ERROR_INFO;

    pSetCmd->trainingErrorInfo.isValid = pLinkTrainingErrorInfo->isValid;
    pSetCmd->runtimeErrorInfo.isValid  = pLinkRuntimeErrorInfo->isValid;

    RM_FLCN_U64_PACK(&pSetCmd->trainingErrorInfo.attemptedTrainingMask0,
                     &pLinkTrainingErrorInfo->attemptedTrainingMask0);
    RM_FLCN_U64_PACK(&pSetCmd->trainingErrorInfo.trainingErrorMask0,
                     &pLinkTrainingErrorInfo->trainingErrorMask0);
    RM_FLCN_U64_PACK(&pSetCmd->runtimeErrorInfo.mask0, &pLinkRuntimeErrorInfo->mask0);

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                 (PRM_FLCN_CMD)&cmd,
                                 NULL,   // pMsg            - not used for now
                                 NULL,   // pPayload        - not used for now
                                 SOE_RM_CMDQ_LOG_ID,
                                 &cmdSeqDesc,
                                 &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s SMBPBI Set Link Error Info command failed. rc:%d\n",
                       __FUNCTION__, status);
        return status;
    }

    return NVL_SUCCESS;
}

