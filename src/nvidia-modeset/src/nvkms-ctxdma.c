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

#include "nvkms-private.h"
#include "nvkms-ctxdma.h"
#include "nvkms-utils.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"

#include "class/cl0002.h" /* NV01_CONTEXT_DMA */

#include <ctrl/ctrl0002.h> /* NV0002_CTRL_CMD_BIND_CONTEXTDMA */

static NvBool GarbageCollectSyncptHelperOneChannel(
    NVDevEvoRec *pDevEvo,
    NvU32 sd,
    NVEvoChannel *pChannel,
    NVEvoSyncpt *pSyncpt,
    NVEvoChannelMask *pIdledChannelMask)
{
    NvBool isChannelIdle = FALSE;

    if ((pChannel->channelMask & pSyncpt->channelMask) == 0) {
        return TRUE;
    }

    if ((*pIdledChannelMask) & pChannel->channelMask) {
        goto done;
    }

    /*! Check whether channel is idle. */
    pDevEvo->hal->IsChannelIdle(pDevEvo, pChannel, sd, &isChannelIdle);

    if (!isChannelIdle) {
        return FALSE;
    }

    /*! record idle channel mask to use in next check */
    *pIdledChannelMask |= pChannel->channelMask;

done:
    pSyncpt->channelMask &= ~pChannel->channelMask;
    return TRUE;
}

static NvBool GarbageCollectSyncptHelperOneSyncpt(
    NVDevEvoRec *pDevEvo,
    NVEvoSyncpt *pSyncpt,
    NVEvoChannelMask *pIdledChannelMask)
{
    NvBool ret = TRUE;
    NvU32 head, sd;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvU32 layer;

            /*!
             * If a given channel isn't idle, continue to check if this syncpt
             * is used on other channels which may be idle.
             */
            for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
                if (!GarbageCollectSyncptHelperOneChannel(
                        pDevEvo,
                        sd,
                        pDevEvo->head[head].layer[layer],
                        pSyncpt,
                        &pIdledChannelMask[sd])) {
                    ret = FALSE;
                }
            }
        }
    }

    return ret;
}

/*!
 * This API try to find free syncpt and then unregisters it.
 * It searches global table, and when finds that all channels using this
 * syncpt are idle then frees it. It makes sure that syncpt is not part
 * of current flip.
 */
static NvBool GarbageCollectSyncpts(
    NVDevEvoRec *pDevEvo)
{
    NvU32 i;
    NvBool freedSyncpt = FALSE;
    NVEvoChannelMask idledChannelMask[NVKMS_MAX_SUBDEVICES] = { 0 };

    if (!pDevEvo->supportsSyncpts) {
        return FALSE;
    }

    for (i = 0; i < NV_SYNCPT_GLOBAL_TABLE_LENGTH; i++) {

        NvBool allLayersIdle = NV_TRUE;

        if (pDevEvo->pAllSyncptUsedInCurrentFlip != NULL) {
            if (pDevEvo->pAllSyncptUsedInCurrentFlip[i]) {
                /*! syncpt is part of current flip, so skip it */
                continue;
            }
        }

        if (pDevEvo->preSyncptTable[i].surfaceDesc.ctxDmaHandle == 0) {
            /*! syncpt isn't registered, so skip it */
            continue;
        }

        allLayersIdle = GarbageCollectSyncptHelperOneSyncpt(
                            pDevEvo,
                            &pDevEvo->preSyncptTable[i],
                            idledChannelMask);

        if (allLayersIdle) {
            /*! Free handles */
            nvRmFreeSyncptHandle(pDevEvo, &pDevEvo->preSyncptTable[i]);
            freedSyncpt = TRUE;
        }
    }

    return freedSyncpt;
}

NvU32 nvCtxDmaBind(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 hCtxDma)
{
    NV0002_CTRL_BIND_CONTEXTDMA_PARAMS params = { };
    NvU32 ret;
    NvBool retryOnlyOnce = TRUE;

    params.hChannel = pChannel->pb.channel_handle;

retryOnce:
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         hCtxDma,
                         NV0002_CTRL_CMD_BIND_CONTEXTDMA,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        /*!
         * syncpts (lazily freed) occupy space in the disp ctxDma hash
         * table, and therefore may cause bind ctxDma to fail.
         * Free any unused syncpts and try again.
         */
        if (retryOnlyOnce) {
            /*! try to free syncpt only once */
            if (GarbageCollectSyncpts(pDevEvo)) {
                retryOnlyOnce = FALSE;
                goto retryOnce;
            }
        }
    }
    return ret;
}

void nvCtxDmaFree(NVDevEvoPtr pDevEvo,
                  NvU32 deviceHandle,
                  NvU32 *hDispCtxDma)
{
    if (*hDispCtxDma) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    deviceHandle, *hDispCtxDma);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, *hDispCtxDma);
        *hDispCtxDma = 0;
    }
}

NvU32 nvCtxDmaAlloc(NVDevEvoPtr pDevEvo, NvU32 *pCtxDmaHandle,
                    NvU32 memoryHandle, NvU32 localCtxDmaFlags, NvU64 limit)
{
    NV_CONTEXT_DMA_ALLOCATION_PARAMS ctxdmaParams = { };
    NvU32 ret, ctxDmaHandle;

    ctxDmaHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    if (ctxDmaHandle == 0) {
        return NVOS_STATUS_ERROR_GENERIC;
    }

    ctxdmaParams.hMemory = memoryHandle;
    ctxdmaParams.flags = localCtxDmaFlags;
    ctxdmaParams.offset = 0;
    ctxdmaParams.limit = limit;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       ctxDmaHandle,
                       NV01_CONTEXT_DMA,
                       &ctxdmaParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, ctxDmaHandle);
    } else {
        *pCtxDmaHandle = ctxDmaHandle;
    }

    return ret;
}
