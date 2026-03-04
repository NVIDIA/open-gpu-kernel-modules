/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#if !defined(__NVIDIA_PUSH_PRIV_IMPORTS_H__)
#define __NVIDIA_PUSH_PRIV_IMPORTS_H__

#include "nvidia-push-types.h"

static inline NvU32 nvPushImportRmApiControl(
    NvPushDevicePtr pDevice,
    NvU32 hObject,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize)
{
    return pDevice->pImports->rmApiControl(pDevice, hObject, cmd,
                                           pParams, paramsSize);
}

static inline NvU32 nvPushImportRmApiAlloc(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hObject,
    NvU32 hClass,
    void *pAllocParams)
{

    return pDevice->pImports->rmApiAlloc(pDevice, hParent, hObject, hClass,
                                         pAllocParams);
}

static inline NvU32 nvPushImportRmApiFree(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hObject)
{
    return pDevice->pImports->rmApiFree(pDevice, hParent, hObject);
}

static inline NvU32 nvPushImportRmApiMapMemoryDma(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    NvU32 flags,
    NvU64 *pDmaOffset)
{
    return pDevice->pImports->rmApiMapMemoryDma(pDevice,
                                                hDevice,
                                                hDma,
                                                hMemory,
                                                offset,
                                                length,
                                                flags,
                                                pDmaOffset);
}

static inline NvU32 nvPushImportRmApiUnmapMemoryDma(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU32 flags,
    NvU64 dmaOffset)
{
    return pDevice->pImports->rmApiUnmapMemoryDma(pDevice,
                                                  hDevice,
                                                  hDma,
                                                  hMemory,
                                                  flags,
                                                  dmaOffset);

}

static inline NvU32 nvPushImportRmApiAllocMemory64(
    NvPushDevicePtr pDevice,
    NvU32 hParent,
    NvU32 hMemory,
    NvU32 hClass,
    NvU32 flags,
    void **ppAddress,
    NvU64 *pLimit)
{
    return pDevice->pImports->rmApiAllocMemory64(pDevice,
                                                 hParent,
                                                 hMemory,
                                                 hClass,
                                                 flags,
                                                 ppAddress,
                                                 pLimit);
}

static inline NvU32 nvPushImportRmApiVidHeapControl(
    NvPushDevicePtr pDevice,
    void *pVidHeapControlParms)
{
    return pDevice->pImports->rmApiVidHeapControl(pDevice,
                                                  pVidHeapControlParms);
}

static inline NvU32 nvPushImportRmApiMapMemory(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    void **ppLinearAddress,
    NvU32 flags)
{
    return pDevice->pImports->rmApiMapMemory(pDevice,
                                             hDevice,
                                             hMemory,
                                             offset,
                                             length,
                                             ppLinearAddress,
                                             flags);
}

static inline NvU32 nvPushImportRmApiUnmapMemory(
    NvPushDevicePtr pDevice,
    NvU32 hDevice,
    NvU32 hMemory,
    void *pLinearAddress,
    NvU32 flags)
{
    return pDevice->pImports->rmApiUnmapMemory(pDevice,
                                               hDevice,
                                               hMemory,
                                               pLinearAddress,
                                               flags);
}

static inline NvU64 nvPushImportGetMilliSeconds(
    NvPushDevicePtr pDevice)
{
    return pDevice->pImports->getMilliSeconds(pDevice);
}

static inline void nvPushImportYield(
    NvPushDevicePtr pDevice)
{
    pDevice->pImports->yield(pDevice);
}

static inline NvBool nvPushImportWaitForEvent(
    NvPushDevicePtr pDevice,
    NvPushImportEvent *pEvent,
    NvU64 timeout)
{
    return pDevice->pImports->waitForEvent(pDevice, pEvent, timeout);
}

static inline void nvPushImportEmptyEventFifo(
    NvPushDevicePtr pDevice,
    NvPushImportEvent *pEvent)
{
    pDevice->pImports->emptyEventFifo(pDevice, pEvent);
}

static inline void nvPushImportChannelErrorOccurred(
    NvPushChannelPtr pChannel,
    NvU32 channelErrCode)
{
    pChannel->pDevice->pImports->channelErrorOccurred(pChannel, channelErrCode);
}

static inline void nvPushImportPushbufferWrapped(
    NvPushChannelPtr pChannel)
{
    pChannel->pDevice->pImports->pushbufferWrapped(pChannel);
}

#define nvPushImportLogError(_pDevice, ...) \
    (_pDevice)->pImports->logError((_pDevice), __VA_ARGS__)

#if defined(DEBUG)
#define nvPushImportLogNvDiss(_pChannel, ...) \
    (_pChannel)->pDevice->pImports->logNvDiss((_pChannel), __VA_ARGS__)
#endif /* DEBUG */

#endif /* __NVIDIA_PUSH_PRIV_IMPORTS_H__ */
