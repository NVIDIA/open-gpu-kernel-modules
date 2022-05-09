/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file    flcnrtosdebug_nvswitch.c
 * @brief   Provides support for capturing RTOS's state in case of Falcon
 *          related failures.
 */

/* ------------------------- Includes --------------------------------------- */

#include "common_nvswitch.h"

#include "flcn/flcn_nvswitch.h"
#include "flcn/flcnable_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

#include "flcn/flcnrtosdebug_nvswitch.h"

/*!
 * Dump the complete stack by iterating from tail to head pointer
 *
 *  @param[in]  device        nvswitch_device pointer
 *  @param[in]  pFlcn         FLCN pointer
 *  @param[in]  queueLogId    Logical ID of the queue
 *  @param[in]  pFlcnCmd      Pointer to the command buffer to read
 *
 */
NV_STATUS
flcnRtosDumpCmdQueue_nvswitch
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            queueLogId,
    RM_FLCN_CMD     *pFlcnCmd
)
{
    FLCNQUEUE  *pQueue;
    NvU32      head;
    NvU32      tail;
    NvU32      tailcache;

    NV_STATUS  status             = NV_OK;
    PFALCON_QUEUE_INFO pQueueInfo = pFlcn->pQueueInfo;

    pQueue = &pQueueInfo->pQueues[queueLogId];
    (void)pQueue->tailGet(device, pFlcn, pQueue, &tail);
    (void)pQueue->headGet(device, pFlcn, pQueue, &head);

    // caching the current tail pointer
    (void)pQueue->tailGet(device, pFlcn, pQueue, &tailcache);

    if (head == tail)
    {
        return status;
    }

    while (tail != head)
    {
        status = flcnQueueReadData(device,pFlcn,
                                    queueLogId,
                                    pFlcnCmd, NV_FALSE);
        NVSWITCH_PRINT(device, ERROR, "%s:" \
                    "Cmd_Dump UnitId %d size %d sq %d ctl %d cmd %d\n",
                    __FUNCTION__,
                    pFlcnCmd->cmdGen.hdr.unitId,
                    pFlcnCmd->cmdGen.hdr.size,
                    pFlcnCmd->cmdGen.hdr.seqNumId,
                    pFlcnCmd->cmdGen.hdr.ctrlFlags,
                    pFlcnCmd->cmdGen.cmd);

        (void)pQueue->tailGet(device, pFlcn, pQueue, &tail);
    }

    // restoring the cached tail pointer
    (void)pQueue->tailSet(device, pFlcn, pQueue, tailcache);

    return status;
}

/*!
 * @brief Populates falcon DMEM pointer in its internal debug info structure
 *
 * @param[in]  device               GPU object pointer
 * @param[in]  pFlcn                FLCN pointer
 * @param[in]  debugInfoDmemOffset  DMEM offset of the falcon debug info
 */
static void
_flcnDbgInfoDmemOffsetSet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU16            debugInfoDmemOffset
)
{
    pFlcn->debug.debugInfoDmemOffset = debugInfoDmemOffset;
}

void
flcnRtosSetupHal
(
    FLCN   *pFlcn
)
{
    flcn_hal *pHal = pFlcn->pHal;

    pHal->dbgInfoDmemOffsetSet  = _flcnDbgInfoDmemOffsetSet_IMPL;
}

