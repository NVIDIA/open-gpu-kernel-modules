/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi.h"


static NV_STATUS _rmapiAlloc_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                       NvHandle *phObject, NvU32 hClass, void *pAllocParams, NvU32 paramsSize)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiAllocWithHandle_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                 NvHandle hObject, NvU32 hClass, void *pAllocParams, NvU32 paramsSize)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiAllocWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                  NvHandle *phObject, NvU32 hClass, NvP64 pAllocParams, NvU32 paramsSize,
                                  NvU32 flags, NvP64 pRightsRequested, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiFree_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiFreeWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                                 NvU32 flags, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiDisableClients_STUB(RM_API *pRmApi, NvHandle *phClientList, NvU32 numClients)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiDisableClientsWithSecInfo_STUB(RM_API *pRmApi, NvHandle *phClientList,
                                        NvU32 numClients, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiControl_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                         void *pParams, NvU32 paramsSize)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiControlWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                    NvP64 pParams, NvU32 paramsSize, NvU32 flags, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiControlPrefetch_STUB(RM_API *pRmApi, NvU32 cmd)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiDupObject_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hParent, NvHandle *phObject,
                           NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiDupObjectWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                      NvHandle *phObject, NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags,
                                      API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiShare_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                       RS_SHARE_POLICY *pSharePolicy)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiShareWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                                  RS_SHARE_POLICY *pSharePolicy, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiMapToCpu_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                          NvU64 offset, NvU64 length, void **ppCpuVirtAddr, NvU32 flags)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiMapToCpuWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                                     NvU64 offset, NvU64 length, NvP64 *ppCpuVirtAddr, NvU32 flags, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiUnmapFromCpu_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory, void *pLinearAddress,
                              NvU32 flags, NvU32 ProcessId)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiUnmapFromCpuWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                                         NvP64 pLinearAddress, NvU32 flags, NvU32 ProcessId, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiMap_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx, NvHandle hMemory,
                     NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiMapWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx, NvHandle hMemory,
                                NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiUnmap_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx,
                       NvU32 flags, NvU64 dmaOffset, NvU64 size)
{
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rmapiUnmapWithSecInfo_STUB(RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx,
                                  NvU32 flags, NvU64 dmaOffset, NvU64 size, API_SECURITY_INFO *pSecInfo)
{
    return NV_ERR_NOT_SUPPORTED;
}

void rmapiInitStubInterface(RM_API *pRmApi)
{
    portMemSet(pRmApi, 0, sizeof(*pRmApi));

    pRmApi->Alloc                        = _rmapiAlloc_STUB;
    pRmApi->AllocWithHandle              = _rmapiAllocWithHandle_STUB;
    pRmApi->AllocWithSecInfo             = _rmapiAllocWithSecInfo_STUB;
    pRmApi->Free                         = _rmapiFree_STUB;
    pRmApi->FreeWithSecInfo              = _rmapiFreeWithSecInfo_STUB;
    pRmApi->DisableClients               = _rmapiDisableClients_STUB;
    pRmApi->DisableClientsWithSecInfo    = _rmapiDisableClientsWithSecInfo_STUB;
    pRmApi->Control                      = _rmapiControl_STUB;
    pRmApi->ControlWithSecInfo           = _rmapiControlWithSecInfo_STUB;
    pRmApi->ControlPrefetch              = _rmapiControlPrefetch_STUB;
    pRmApi->DupObject                    = _rmapiDupObject_STUB;
    pRmApi->DupObjectWithSecInfo         = _rmapiDupObjectWithSecInfo_STUB;
    pRmApi->Share                        = _rmapiShare_STUB;
    pRmApi->ShareWithSecInfo             = _rmapiShareWithSecInfo_STUB;
    pRmApi->MapToCpu                     = _rmapiMapToCpu_STUB;
    pRmApi->MapToCpuWithSecInfo          = _rmapiMapToCpuWithSecInfo_STUB;
    pRmApi->UnmapFromCpu                 = _rmapiUnmapFromCpu_STUB;
    pRmApi->UnmapFromCpuWithSecInfo      = _rmapiUnmapFromCpuWithSecInfo_STUB;
    pRmApi->Map                          = _rmapiMap_STUB;
    pRmApi->MapWithSecInfo               = _rmapiMapWithSecInfo_STUB;
    pRmApi->Unmap                        = _rmapiUnmap_STUB;
    pRmApi->UnmapWithSecInfo             = _rmapiUnmapWithSecInfo_STUB;
}
