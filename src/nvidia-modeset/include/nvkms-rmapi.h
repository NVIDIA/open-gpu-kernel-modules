/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_RMAPI_H__

#define __NVKMS_RMAPI_H__

#include "nvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

NvU32 nvRmApiAlloc(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObject,
    NvU32 hClass,
    void *pAllocParams);

NvU32 nvRmApiAllocMemory64(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hMemory,
    NvU32 hClass,
    NvU32 flags,
    void **ppAddress,
    NvU64 *pLimit);

NvU32 nvRmApiControl(
    NvU32 hClient,
    NvU32 hObject,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize);

NvU32 nvRmApiDupObject2(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 *hObjectDest,
    NvU32 hClientSrc,
    NvU32 hObjectSrc,
    NvU32 flags);

NvU32 nvRmApiDupObject(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObjectDest,
    NvU32 hClientSrc,
    NvU32 hObjectSrc,
    NvU32 flags);

NvU32 nvRmApiFree(
    NvU32 hClient,
    NvU32 hParent,
    NvU32 hObject);

NvU32 nvRmApiVidHeapControl(
    void *pVidHeapControlParams);

NvU32 nvRmApiMapMemory(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    void **ppLinearAddress,
    NvU32 flags);

NvU32 nvRmApiUnmapMemory(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hMemory,
    const void *pLinearAddress,
    NvU32 flags);

NvU32 nvRmApiMapMemoryDma(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU64 offset,
    NvU64 length,
    NvU32 flags,
    NvU64 *pDmaOffset);

NvU32 nvRmApiUnmapMemoryDma(
    NvU32 hClient,
    NvU32 hDevice,
    NvU32 hDma,
    NvU32 hMemory,
    NvU32 flags,
    NvU64 dmaOffset);

#ifdef __cplusplus
};
#endif

#endif /* __NVKMS_RMAPI_H__ */
