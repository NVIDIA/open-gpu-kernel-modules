/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _ENTRYPOINTS_H_
#define _ENTRYPOINTS_H_

//
// Internal handlers for RM APIs
//

NV_STATUS
rmapiAlloc
(
    RM_API      *pRmApi,
    NvHandle     hClient,
    NvHandle     hParent,
    NvHandle    *phObject,
    NvU32        hClass,
    void        *pAllocParams,
    NvU32        paramsSize
);

NV_STATUS
rmapiAllocWithHandle
(
    RM_API      *pRmApi,
    NvHandle     hClient,
    NvHandle     hParent,
    NvHandle     hObject,
    NvU32        hClass,
    void        *pAllocParams,
    NvU32        paramsSize
);

NV_STATUS
rmapiAllocWithSecInfo
(
    RM_API              *pRmApi,
    NvHandle             hClient,
    NvHandle             hParent,
    NvHandle            *phObject,
    NvU32                hClass,
    NvP64                pAllocParams,
    NvU32                paramsSize,
    NvU32                flags,
    NvP64                pRightsRequired,
    API_SECURITY_INFO   *pSecInfo
);

NV_STATUS
rmapiAllocWithSecInfoTls
(
    RM_API              *pRmApi,
    NvHandle             hClient,
    NvHandle             hParent,
    NvHandle            *phObject,
    NvU32                hClass,
    NvP64                pAllocParams,
    NvU32                paramsSize,
    NvU32                flags,
    NvP64                pRightsRequired,
    API_SECURITY_INFO   *pSecInfo
);

NV_STATUS
rmapiFree
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hObject
);

NV_STATUS
rmapiFreeWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiFreeWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiDisableClients
(
    RM_API   *pRmApi,
    NvHandle *phClientList,
    NvU32     numClients
);

NV_STATUS
rmapiDisableClientsWithSecInfo
(
    RM_API   *pRmApi,
    NvHandle *phClientList,
    NvU32     numClients,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiDisableClientsWithSecInfoTls
(
    RM_API   *pRmApi,
    NvHandle *phClientList,
    NvU32     numClients,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiControl
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hObject,
    NvU32     cmd,
    void     *pParams,
    NvU32     paramsSize
);

NV_STATUS
rmapiControlWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              cmd,
    NvP64              pParams,
    NvU32              paramsSize,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiControlWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    NvU32              cmd,
    NvP64              pParams,
    NvU32              paramsSize,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiDupObject
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hParent,
    NvHandle *phObject,
    NvHandle  hClientSrc,
    NvHandle  hObjectSrc,
    NvU32     flags
);

NV_STATUS
rmapiDupObjectWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hParent,
    NvHandle          *phObject,
    NvHandle           hClientSrc,
    NvHandle           hObjectSrc,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiDupObjectWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hParent,
    NvHandle          *phObject,
    NvHandle           hClientSrc,
    NvHandle           hObjectSrc,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiShare
(
    RM_API          *pRmApi,
    NvHandle         hClient,
    NvHandle         hObject,
    RS_SHARE_POLICY *pSharePolicy
);

NV_STATUS
rmapiShareWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    RS_SHARE_POLICY   *pSharePolicy,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiShareWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    RS_SHARE_POLICY   *pSharePolicy,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiMapToCpu
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemory,
    NvU64     offset,
    NvU64     length,
    void    **ppCpuVirtAddr,
    NvU32     flags
);

NV_STATUS
rmapiMapToCpuWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiMapToCpuWithSecInfoV2
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32             *flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiMapToCpuWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
);
NV_STATUS
rmapiMapToCpuWithSecInfoTlsV2
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvP64             *ppCpuVirtAddr,
    NvU32             *flags,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiUnmapFromCpu
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemory,
    void     *pLinearAddress,
    NvU32     flags,
    NvU32     ProcessId
);

NV_STATUS
rmapiUnmapFromCpuWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvP64              pLinearAddress,
    NvU32              flags,
    NvU32              ProcessId,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiUnmapFromCpuWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemory,
    NvP64              pLinearAddress,
    NvU32              flags,
    NvU32              ProcessId,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiMap
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemCtx,
    NvHandle  hMemory,
    NvU64     offset,
    NvU64     length,
    NvU32     flags,
    NvU64    *pDmaOffset
);

NV_STATUS
rmapiMapWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvU32              flags,
    NvU64             *pDmaOffset,
    API_SECURITY_INFO *pSecInfo
);


NV_STATUS
rmapiMapWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvHandle           hMemory,
    NvU64              offset,
    NvU64              length,
    NvU32              flags,
    NvU64             *pDmaOffset,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiUnmap
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hDevice,
    NvHandle  hMemCtx,
    NvU32     flags,
    NvU64     dmaOffset,
    NvU64     size
);

NV_STATUS
rmapiUnmapWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvU32              flags,
    NvU64              dmaOffset,
    NvU64              size,
    API_SECURITY_INFO *pSecInfo
);

NV_STATUS
rmapiUnmapWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hDevice,
    NvHandle           hMemCtx,
    NvU32              flags,
    NvU64              dmaOffset,
    NvU64              size,
    API_SECURITY_INFO *pSecInfo
);

#endif // _ENTRYPOINTS_H_

