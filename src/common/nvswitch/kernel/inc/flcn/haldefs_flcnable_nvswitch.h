/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _HALDEFS_FLCNABLE_NVSWITCH_H_
#define _HALDEFS_FLCNABLE_NVSWITCH_H_

#include "nvstatus.h"
#include "flcnifcmn.h"

struct nvswitch_device;
struct NVSWITCH_TIMEOUT;
struct FLCNABLE;
struct FALCON_EXTERNAL_CONFIG;
struct FLCN_QMGR_SEQ_INFO;
union RM_FLCN_MSG;
union RM_FLCN_CMD;
struct ENGINE_DESCRIPTOR_TYPE;

typedef struct {
    NvU8                        (*readCoreRev)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable);
    void                        (*getExternalConfig)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        struct FALCON_EXTERNAL_CONFIG  *pConfig);
    void                        (*ememCopyFrom)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        NvU32                           src,
                                        NvU8                           *pDst,
                                        NvU32                           sizeBytes,
                                        NvU8                            port);
    void                        (*ememCopyTo)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        NvU32                           dst,
                                        NvU8                           *pSrc,
                                        NvU32                           sizeBytes,
                                        NvU8                            port);
    NV_STATUS                   (*handleInitEvent)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        union RM_FLCN_MSG              *pGenMsg);
    struct FLCN_QMGR_SEQ_INFO*  (*queueSeqInfoGet)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        NvU32                           seqIndex);
    void                        (*queueSeqInfoClear)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        struct FLCN_QMGR_SEQ_INFO      *pSeqInfo);
    void                        (*queueSeqInfoFree)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        struct FLCN_QMGR_SEQ_INFO      *pSeqInfo);
    NvBool                      (*queueCmdValidate)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        union RM_FLCN_CMD              *pCmd,
                                        union RM_FLCN_MSG              *pMsg,
                                        void                           *pPayload,
                                        NvU32                           queueIdLogical);
    NV_STATUS                   (*queueCmdPostExtension)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        union RM_FLCN_CMD              *pCmd,
                                        union RM_FLCN_MSG              *pMsg,
                                        void                           *pPayload,
                                        struct NVSWITCH_TIMEOUT        *pTimeout,
                                        struct FLCN_QMGR_SEQ_INFO      *pSeqInfo);
    void                        (*postDiscoveryInit)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable);



    NV_STATUS                   (*construct)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable);
    void                        (*destruct)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable);
    void                        (*fetchEngines)(
                                        struct nvswitch_device         *device,
                                        struct FLCNABLE                *pFlcnable,
                                        struct ENGINE_DESCRIPTOR_TYPE  *pEngDescUc,
                                        struct ENGINE_DESCRIPTOR_TYPE  *pEngDescBc);

} flcnable_hal;

#endif //_HALDEFS_FLCNABLE_NVSWITCH_H_
