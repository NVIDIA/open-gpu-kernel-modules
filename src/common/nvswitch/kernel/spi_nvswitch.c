/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmsoecmdif.h"
#include "spi_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

NvlStatus
nvswitch_spi_init
(
    nvswitch_device *device
)
{
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;
    FLCN               *pFlcn;

    if (!device->pSoe)
    {
        return -NVL_ERR_INVALID_STATE;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_SPI;
    cmd.hdr.size   = RM_FLCN_QUEUE_HDR_SIZE + sizeof(RM_SOE_SPI_CMD);
    cmd.cmd.spi.cmdType = RM_SOE_SPI_INIT;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1MSEC_IN_NS * 30, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: SPI INIT failed. rc:%d\n",
                        __FUNCTION__, status);
    }

    return status;
}
