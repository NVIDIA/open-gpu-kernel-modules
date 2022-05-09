/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "flcn/haldefs_flcnable_nvswitch.h"
#include "flcn/flcnable_nvswitch.h"

#include "flcnifcmn.h"

#include "export_nvswitch.h"
#include "common_nvswitch.h"

typedef struct FALCON_EXTERNAL_CONFIG FALCON_EXTERNAL_CONFIG, *PFALCON_EXTERNAL_CONFIG;
typedef struct FLCN_QMGR_SEQ_INFO FLCN_QMGR_SEQ_INFO, *PFLCN_QMGR_SEQ_INFO;
typedef union  RM_FLCN_CMD RM_FLCN_CMD, *PRM_FLCN_CMD;
typedef union  RM_FLCN_MSG RM_FLCN_MSG, *PRM_FLCN_MSG;
typedef struct ENGINE_DESCRIPTOR_TYPE ENGINE_DESCRIPTOR_TYPE, *PENGINE_DESCRIPTOR_TYPE;


// OBJECT Interfaces
NvU8
flcnableReadCoreRev
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->readCoreRev != (void *)0);
    return pFlcnable->pHal->readCoreRev(device, pFlcnable);
}

void
flcnableGetExternalConfig
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    PFALCON_EXTERNAL_CONFIG pConfig
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->getExternalConfig != (void *)0);
    pFlcnable->pHal->getExternalConfig(device, pFlcnable, pConfig);
}

void
flcnableEmemCopyFrom
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            src,
    NvU8            *pDst,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->ememCopyFrom != (void *)0);
    pFlcnable->pHal->ememCopyFrom(device, pFlcnable, src, pDst, sizeBytes, port);
}

void
flcnableEmemCopyTo
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->ememCopyTo != (void *)0);
    pFlcnable->pHal->ememCopyTo(device, pFlcnable, dst, pSrc, sizeBytes, port);
}

NV_STATUS
flcnableHandleInitEvent
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    RM_FLCN_MSG     *pGenMsg
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->handleInitEvent != (void *)0);
    return pFlcnable->pHal->handleInitEvent(device, pFlcnable, pGenMsg);
}

PFLCN_QMGR_SEQ_INFO
flcnableQueueSeqInfoGet
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            seqIndex
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->queueSeqInfoGet != (void *)0);
    return pFlcnable->pHal->queueSeqInfoGet(device, pFlcnable, seqIndex);
}

void
flcnableQueueSeqInfoClear
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->queueSeqInfoClear != (void *)0);
    pFlcnable->pHal->queueSeqInfoClear(device, pFlcnable, pSeqInfo);
}

void
flcnableQueueSeqInfoFree
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->queueSeqInfoFree != (void *)0);
    pFlcnable->pHal->queueSeqInfoFree(device, pFlcnable, pSeqInfo);
}

NvBool
flcnableQueueCmdValidate
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    RM_FLCN_CMD     *pCmd,
    RM_FLCN_MSG     *pMsg,
    void            *pPayload,
    NvU32            queueIdLogical
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->queueCmdValidate != (void *)0);
    return pFlcnable->pHal->queueCmdValidate(device, pFlcnable, pCmd, pMsg, pPayload, queueIdLogical);
}

NV_STATUS
flcnableQueueCmdPostExtension
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    RM_FLCN_CMD        *pCmd,
    RM_FLCN_MSG        *pMsg,
    void               *pPayload,
    NVSWITCH_TIMEOUT   *pTimeout,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->queueCmdPostExtension != (void *)0);
    return pFlcnable->pHal->queueCmdPostExtension(device, pFlcnable, pCmd, pMsg, pPayload, pTimeout, pSeqInfo);
}

void
flcnablePostDiscoveryInit
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->postDiscoveryInit != (void *)0);
    pFlcnable->pHal->postDiscoveryInit(device, pFlcnable);
}



// HAL Interfaces
NV_STATUS
flcnableConstruct_HAL
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->construct != (void *)0);
    return pFlcnable->pHal->construct(device, pFlcnable);
}

void
flcnableDestruct_HAL
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->destruct != (void *)0);
    pFlcnable->pHal->destruct(device, pFlcnable);
}

void
flcnableFetchEngines_HAL
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescUc,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescBc
)
{
    NVSWITCH_ASSERT(pFlcnable->pHal->fetchEngines != (void *)0);
    pFlcnable->pHal->fetchEngines(device, pFlcnable, pEngDescUc, pEngDescBc);
}
