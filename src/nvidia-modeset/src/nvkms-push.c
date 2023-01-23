/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvkms-push.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h"

#include "nvidia-push-methods.h"
#include "nvidia-push-utils.h"

/*
 * Wrapper functions needed by nvidia-push.
 */
static NvU32 NvPushImportRmApiControl(
    NvPushDevicePtr pDevice,
    NvU32 hObject,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize)
{
    return nvRmApiControl(
            nvEvoGlobal.clientHandle,
            hObject,
            cmd,
            pParams,
            paramsSize);
}

static NvU32 NvPushImportRmApiAlloc(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hObject,
    NvU32 hClass,
    void *pAllocParams)
{
    return nvRmApiAlloc(
            nvEvoGlobal.clientHandle,
            hParent,
            hObject,
            hClass,
            pAllocParams);
}

static NvU32 NvPushImportRmApiFree(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hObject)
{
    return nvRmApiFree(
            nvEvoGlobal.clientHandle,
            hParent,
            hObject);
}

static NvU32 NvPushImportRmApiMapMemoryDma(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    NvU32 flags,
    NvU64 *pDmaOffset)
{
    return nvRmApiMapMemoryDma(
            nvEvoGlobal.clientHandle,
            hDevice,
            hDma,
            hMemory,
            offset,
            length,
            flags,
            pDmaOffset);
}

static NvU32 NvPushImportRmApiUnmapMemoryDma(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU32 flags,
    NvU64 dmaOffset)
{
    return nvRmApiUnmapMemoryDma(
            nvEvoGlobal.clientHandle,
            hDevice,
            hDma,
            hMemory,
            flags,
            dmaOffset);
}

static NvU32 NvPushImportRmApiAllocMemory64(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hMemory,
    NvU32 hClass,
    NvU32 flags,
    void **ppAddress,
    NvU64 *pLimit)
{
    return nvRmApiAllocMemory64(
            nvEvoGlobal.clientHandle,
            hParent,
            hMemory,
            hClass,
            flags,
            ppAddress,
            pLimit);
}

static NvU32 NvPushImportRmApiVidHeapControl(
    NvPushDevicePtr pDevice,
    void *pVidHeapControlParms)
{
    return nvRmApiVidHeapControl(pVidHeapControlParms);
}

static NvU32 NvPushImportRmApiMapMemory(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    void **ppLinearAddress,
    NvU32 flags)
{
    return nvRmApiMapMemory(
            nvEvoGlobal.clientHandle,
            hDevice,
            hMemory,
            offset,
            length,
            ppLinearAddress,
            flags);
}

static NvU32 NvPushImportRmApiUnmapMemory(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hMemory,
    void *pLinearAddress,
    NvU32 flags)
{
    return nvRmApiUnmapMemory(
            nvEvoGlobal.clientHandle,
            hDevice,
            hMemory,
            pLinearAddress,
            flags);
}

static NvU64 NvPushImportGetMilliSeconds(
    NvPushDevicePtr pDevice)
{
    return (nvkms_get_usec() + 500) / 1000;
}

static void NvPushImportYield(
    NvPushDevicePtr pDevice)
{
    nvkms_yield();
}

static NvBool NvPushImportWaitForEvent(
    NvPushDevicePtr pDevice,
    NvPushImportEvent *pEvent,
    NvU64 timeout)
{
    return FALSE;
}

static void NvPushImportEmptyEventFifo(
    NvPushDevicePtr pDevice,
    NvPushImportEvent *pEvent)
{
    return;
}

static void NvPushImportChannelErrorOccurred(
    NvPushChannelPtr pChannel,
    NvU32 channelErrCode)
{
    /* XXX TODO: implement me */
}

static void NvPushImportPushbufferWrapped(
    NvPushChannelPtr pChannel)
{
    /* XXX TODO: implement me */
}

static void NvPushImportLogError(
    NvPushDevicePtr pDevice,
    const char *fmt, ...)
{
    const NVDevEvoRec *pDevEvo = pDevice->hostDevice;

    nvAssert(pDevEvo);

    va_list ap;
    va_start(ap, fmt);
    nvVEvoLog(EVO_LOG_ERROR, pDevEvo->gpuLogIndex, fmt, ap);
    va_end(ap);
}

#if defined(DEBUG)
static void NvPushImportLogNvDiss(
    NvPushChannelPtr pChannel,
    const char *fmt, ...)
{
    /* XXX TODO: implement me */
}
#endif /* DEBUG */

static const NvPushImports NvKmsNvPushImports = {
    NvPushImportRmApiControl,          /* rmApiControl */
    NvPushImportRmApiAlloc,            /* rmApiAlloc */
    NvPushImportRmApiFree,             /* rmApiFree */
    NvPushImportRmApiMapMemoryDma,     /* rmApiMapMemoryDma */
    NvPushImportRmApiUnmapMemoryDma,   /* rmApiUnmapMemoryDma */
    NvPushImportRmApiAllocMemory64,    /* rmApiAllocMemory64 */
    NvPushImportRmApiVidHeapControl,   /* rmApiVidHeapControl */
    NvPushImportRmApiMapMemory,        /* rmApiMapMemory */
    NvPushImportRmApiUnmapMemory,      /* rmApiUnmapMemory */
    NvPushImportGetMilliSeconds,       /* getMilliSeconds */
    NvPushImportYield,                 /* yield */
    NvPushImportWaitForEvent,          /* waitForEvent */
    NvPushImportEmptyEventFifo,        /* emptyEventFifo */
    NvPushImportChannelErrorOccurred,  /* channelErrorOccurred */
    NvPushImportPushbufferWrapped,     /* pushbufferWrapped */
    NvPushImportLogError,              /* logError */
#if defined(DEBUG)
    NvPushImportLogNvDiss,             /* logNvDiss */
#endif
};

NvBool nvAllocNvPushDevice(NVDevEvoPtr pDevEvo)
{
    NvPushAllocDeviceParams params = { };
    NvU32 sd, h;

    params.hostDevice = pDevEvo;
    params.pImports = &NvKmsNvPushImports;
    params.clientHandle = nvEvoGlobal.clientHandle;
    params.subDevice[0].deviceHandle = pDevEvo->deviceHandle;
    params.numSubDevices = pDevEvo->numSubDevices;

    params.numClasses = pDevEvo->numClasses;
    params.supportedClasses = pDevEvo->supportedClasses;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        params.subDevice[sd].handle = pDevEvo->pSubDevices[sd]->handle;
    }

    params.amodel.config = NV_AMODEL_NONE;
    params.isTegra = FALSE;
    params.subDevice[0].gpuVASpace = pDevEvo->nvkmsGpuVASpace;

    ct_assert(sizeof(params.handlePool) ==
              sizeof(pDevEvo->nvPush.handlePool));

    for (h = 0; h < ARRAY_LEN(pDevEvo->nvPush.handlePool); h++) {
        pDevEvo->nvPush.handlePool[h] =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
        params.handlePool[h] = pDevEvo->nvPush.handlePool[h];
    }

    if (!nvPushAllocDevice(&params, &pDevEvo->nvPush.device)) {
        nvFreeNvPushDevice(pDevEvo);
        return FALSE;
    }

    return TRUE;
}

void nvFreeNvPushDevice(NVDevEvoPtr pDevEvo)
{
    NvU32 h;

    nvPushFreeDevice(&pDevEvo->nvPush.device);

    for (h = 0; h < ARRAY_LEN(pDevEvo->nvPush.handlePool); h++) {
        if (pDevEvo->nvPush.handlePool[h] != 0) {
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pDevEvo->nvPush.handlePool[h]);
            pDevEvo->nvPush.handlePool[h] = 0;
        }
    }
}
