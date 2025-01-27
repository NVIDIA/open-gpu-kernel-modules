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
#if defined(NV_SMG_IN_NVKMS)
#  include "nvkms-utils.h"
#else
#  include <stdlib.h>
#  include <string.h>
#endif


#include "nvmisc.h"
#include "nv_smg.h"

#include "class/clc637.h"
#include "class/clc638.h"
#include <ctrl/ctrl2080/ctrl2080gpu.h>
#include <ctrl/ctrlc637.h>
#include <ctrl/ctrlc638.h>

/*
 * Set up SMG for the given subdevice. Handles partition allocation and
 * selection. Returns true if this subdevice is equipped to do graphics.
 * That is, either when not in MIG-mode at all, or when MIG-mode is enabled
 * and we were able to successfully set up an SMG partition for doing
 * graphics on this subdevice. Returns false otherwise when any kind of
 * unrecoverable error condition that means broken graphics is encountered.
 */
NvBool NVSubdevSMGSetPartition(void *ctx,
                               NvU32 subdevHandle,
                               const char *computeInstUuid,
                               NvU32 gpuInstSubscriptionHdl,
                               NvU32 computeInstSubscriptionHdl,
                               NVSubdevSMGRMControl rmControl,
                               NVSubdevSMGRMAlloc rmAlloc,
                               NVSubdevSMGRMFree rmFree)
{
    /*
     * These RM parameters can be huge: allocate them from the heap to keep
     * kernel stack usage low. For userspace, allocate on stack.
     */
    struct workspace {
        NV2080_CTRL_GPU_GET_INFO_V2_PARAMS gpuInfoParams;
        NV2080_CTRL_GPU_GET_PARTITIONS_PARAMS getPartParams;
        NVC637_ALLOCATION_PARAMETERS allocParams;
        NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS getExecPartParams;
        NVC638_ALLOCATION_PARAMETERS smgExecAllocParams;
        NVC638_CTRL_GET_UUID_PARAMS getUuidParams;
    } *ws;
    NvU32 res;
    NvU32 smcMode;
    int i;
    int j;

#if defined(NV_SMG_IN_NVKMS)
    /* Allocate workspace from heap. */
    ws = (struct workspace *)nvAlloc(sizeof(*ws));
    if (!ws) {
        return NV_FALSE;
    }
    nvkms_memset(ws, 0, sizeof(*ws));
#else
    /* Allocate workspace from stack. */
    struct workspace _ws;

    ws = &_ws;
    memset(ws, 0, sizeof(*ws));
#endif

    /*
     * Before anything, check explicitly for MIG mode
     */
    ws->gpuInfoParams.gpuInfoListSize = 1;
    ws->gpuInfoParams.gpuInfoList[0].index = NV2080_CTRL_GPU_INFO_INDEX_GPU_SMC_MODE;
    res = rmControl(ctx,
                  subdevHandle,
                  NV2080_CTRL_CMD_GPU_GET_INFO_V2,
                  &ws->gpuInfoParams,
                  sizeof(ws->gpuInfoParams));

    if (res != NV_OK) {
        return NV_FALSE;
    }

#if !defined(NV_SMG_IN_NVKMS)
    /*
     * NOTE: The simplest possible support for N-way MIG for a transitional
     * period. This handles all SMG subscription calls originating from
     * userspace by checking for an environment override for the default MIG
     * device UUID. This is a measure that extends the way 1-way SMG works
     * to enable further testing and sharing between teams. A complete
     * solution of how to negotiate MIG UUIDs between display server and
     * clients is a major step that requires larger design considerations
     * than just few patches.
     */
    if (computeInstUuid == NULL) {
        const char *uuid = getenv("MIG_DEVICE_UUID");

        if (uuid && uuid[0]) {
            computeInstUuid = uuid;
        }
    }
#endif

    /*
     * If we're not in MIG mode then just return true to indicate we're able
     * to do graphics. No MIG mode, no subscriptions.
     */
    smcMode = ws->gpuInfoParams.gpuInfoList[0].data;

    if (smcMode != NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_ENABLED &&
        smcMode != NV2080_CTRL_GPU_INFO_GPU_SMC_MODE_DISABLE_PENDING) {
#if defined(NV_SMG_IN_NVKMS)
        nvFree(ws);
#endif
        return NV_TRUE;
    }

    /*
     * Query available GPU instances (GPU/graphics partitions) on the given
     * subdevice.
     */
    ws->getPartParams.bGetAllPartitionInfo = NV_TRUE;

    res = rmControl(ctx,
                    subdevHandle,
                    NV2080_CTRL_CMD_GPU_GET_PARTITIONS,
                    &ws->getPartParams,
                    sizeof(ws->getPartParams));

    if (res != NV_OK || ws->getPartParams.validPartitionCount == 0) {
#if defined(NV_SMG_IN_NVKMS)
        nvFree(ws);
#endif
        return NV_FALSE;
    }

    /* Look for graphics partitions only. */
    for (i = 0; i < ws->getPartParams.validPartitionCount; i++) {
        const NV2080_CTRL_GPU_GET_PARTITION_INFO *pinfo =
            &ws->getPartParams.queryPartitionInfo[i];
        const NvU32 gfxSize = DRF_VAL(
            2080_CTRL_GPU, _PARTITION_FLAG, _GFX_SIZE, pinfo->partitionFlag);

        if (gfxSize == NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE) {
            continue;
        }

        /*
         * OK, an SMG partition was found: subscribe to it in order to be
         * able to investigate further.
         */
        ws->allocParams.swizzId = pinfo->swizzId;

        res = rmAlloc(ctx,
                      subdevHandle,
                      gpuInstSubscriptionHdl,
                      AMPERE_SMC_PARTITION_REF,
                      &ws->allocParams);

        if (res != NV_OK) {
#if defined(NV_SMG_IN_NVKMS)
            nvFree(ws);
#endif
            return NV_FALSE;
        }

        /*
         * Next, find out about compute instances (exec partitions) under
         * the current graphics partition.
         */
        res = rmControl(ctx,
                        gpuInstSubscriptionHdl,
                        NVC637_CTRL_CMD_EXEC_PARTITIONS_GET,
                        &ws->getExecPartParams,
                        sizeof(ws->getExecPartParams));

        if (res != NV_OK) {
            rmFree(ctx, subdevHandle, gpuInstSubscriptionHdl);
#if defined(NV_SMG_IN_NVKMS)
            nvFree(ws);
#endif
            return NV_FALSE;
        }

        /*
         * Loop through all compute instances (exec partitions) and try
         * subscribing to each one in turn to determine a potential match.
         */
        for (j = 0; j < ws->getExecPartParams.execPartCount; j++) {
            ws->smgExecAllocParams.execPartitionId =
                ws->getExecPartParams.execPartId[j];

            res = rmAlloc(ctx,
                          gpuInstSubscriptionHdl,
                          computeInstSubscriptionHdl,
                          AMPERE_SMC_EXEC_PARTITION_REF,
                          &ws->smgExecAllocParams);

            if (res == NV_OK) {
                /*
                 * For 1-way SMG the requested compute instance UUID should
                 * be null. In this case we just pick the first available
                 * compute instance. For N-way SMG the UUID can vary based
                 * on what the application wants and we want to find the
                 * first matching gpu instance.
                 */
                NvBool matchingInstance = (computeInstUuid == NULL);

                if (computeInstUuid != NULL) {
                    res = rmControl(ctx,
                                    computeInstSubscriptionHdl,
                                    NVC638_CTRL_CMD_GET_UUID,
                                    &ws->getUuidParams,
                                    sizeof(ws->getUuidParams));
                    if (res == NV_OK) {
#if defined(NV_SMG_IN_NVKMS)
                        matchingInstance = (nvkms_strcmp
                            (computeInstUuid, ws->getUuidParams.uuidStr) == 0);
#else
                        matchingInstance = (strcmp
                            (computeInstUuid, ws->getUuidParams.uuidStr) == 0);
#endif
                    }
                }

                /*
                 * Ok, found a matching compute instance. The current GPU
                 * (GPU partition) instance gets thus implicitly selected as
                 * we return and leave the subcription objects attached to
                 * the subdevice.
                 */
                if (matchingInstance) {
#if defined(NV_SMG_IN_NVKMS)
                    nvFree(ws);
#endif
                    return NV_TRUE;
                }
            }

            rmFree(ctx, gpuInstSubscriptionHdl, computeInstSubscriptionHdl);
        }

        rmFree(ctx, subdevHandle, gpuInstSubscriptionHdl);
    }

#if defined(NV_SMG_IN_NVKMS)
    nvFree(ws);
#endif

    return NV_FALSE;
}
