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

#include "common_nvswitch.h"

#include "flcn/flcn_nvswitch.h"
#include "flcn/flcnable_nvswitch.h"

static void flcnSetupIpHal(nvswitch_device *device, PFLCN pFlcn);

/*!
 * @brief   Get the falcon core revision and subversion.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 *
 * @return the falcon core revision in the format of NV_FLCN_CORE_REV_X_Y.
 */
static NvU8
_flcnCoreRevisionGet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    if (pFlcn->coreRev == 0x00)
    {
        // Falcon core revision has not yet been set.  Set it now.
        flcnGetCoreInfo_HAL(device, pFlcn);
    }

    return pFlcn->coreRev;
}

/*!
 *  @brief Mark the falcon as not ready and inaccessible from RM.
 *  osHandleGpuSurpriseRemoval will use this routine to prevent access to the
 *  Falcon, which could crash due to absense of GPU, during driver cleanup.
 *
 *  @param[in] device nvswitch_device pointer
 *  @param[in] pFlcn  FLCN pointer
 *
 *  @returns nothing
 */
static void
_flcnMarkNotReady_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    pFlcn->bOSReady = NV_FALSE;
}

/*!
 * Retrieves the current head pointer for given physical command queue index.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   pQueue  Pointer to the queue
 * @param[out]  pHead   Pointer to write with the queue's head pointer
 *
 * @return 'NV_OK' if head value was successfully retrieved.
 */
static NV_STATUS
_flcnCmdQueueHeadGet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pHead
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->cmdQHeadSize);
    NVSWITCH_ASSERT(pHead != NULL);

    *pHead = flcnRegRead_HAL(device, pFlcn,
                                    (pQueueInfo->cmdQHeadBaseAddress +
                                    (pQueue->queuePhyId * pQueueInfo->cmdQHeadStride)));
    return NV_OK;
}

/*!
 * Sets the head pointer for the given physical command queue index.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  head    The desired head value for the queue
 *
 * @return 'NV_OK' if the head value was successfully set.
 */
static NV_STATUS
_flcnCmdQueueHeadSet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            head
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->cmdQHeadSize);

    flcnRegWrite_HAL(device, pFlcn,
                            (pQueueInfo->cmdQHeadBaseAddress +
                            (pQueue->queuePhyId * pQueueInfo->cmdQHeadStride)),
                            head);
    return NV_OK;
}

/*!
 * Retrieves the current tail pointer for given physical command queue index.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   pQueue  Pointer to the queue
 * @param[out]  pTail   Pointer to write with the queue's tail value
 *
 * @return 'NV_OK' if the tail value was successfully retrieved.
 */
static NV_STATUS
_flcnCmdQueueTailGet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pTail
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->cmdQTailSize);
    NVSWITCH_ASSERT(pTail != NULL);

    *pTail = flcnRegRead_HAL(device, pFlcn,
                                    (pQueueInfo->cmdQTailBaseAddress +
                                    (pQueue->queuePhyId * pQueueInfo->cmdQTailStride)));
    return NV_OK;
}

/*!
 * Set the Command Queue tail pointer.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  tail    The desired tail value
 *
 * @return 'NV_OK' if the tail value was successfully set.
 */
static NV_STATUS
_flcnCmdQueueTailSet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            tail
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->cmdQTailSize);

    flcnRegWrite_HAL(device, pFlcn,
                            (pQueueInfo->cmdQTailBaseAddress +
                            (pQueue->queuePhyId * pQueueInfo->cmdQTailStride)),
                            tail);
    return NV_OK;
}

/*!
 * Retrieve the current Message Queue Head pointer.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  pHead   Pointer to write with the queue's head value
 *
 * @return 'NV_OK' if the queue's head value was successfully retrieved.
 */
static NV_STATUS
_flcnMsgQueueHeadGet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pHead
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->msgQHeadSize);
    NVSWITCH_ASSERT(pHead != NULL);

    *pHead = flcnRegRead_HAL(device, pFlcn,
                                    (pQueueInfo->msgQHeadBaseAddress +
                                    (pQueue->queuePhyId * pQueueInfo->msgQHeadStride)));
    return NV_OK;
}

/*!
 * Set the Message Queue Head pointer.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  head    The desired head value
 *
 * @return 'NV_OK' if the head value was successfully set.
 */
static NV_STATUS
_flcnMsgQueueHeadSet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            head
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->msgQHeadSize);

    flcnRegWrite_HAL(device, pFlcn,
                            (pQueueInfo->msgQHeadBaseAddress +
                            (pQueue->queuePhyId * pQueueInfo->msgQHeadStride)),
                            head);
    return NV_OK;
}

/*!
 * Retrieve the current Message Queue Tail pointer.
 *
 * @param[in]   device  nvswitch device pointer
 * @param[in]   pFlcn   FLCN object pointer
 * @param[in]   pQueue  Pointer to the queue
 * @param[out]  pTail   Pointer to write with the message queue's tail value
 *
 * @return 'NV_OK' if the tail value was successfully retrieved.
 */
static NV_STATUS
_flcnMsgQueueTailGet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32           *pTail
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->msgQTailSize);
    NVSWITCH_ASSERT(pTail != NULL);

    *pTail = flcnRegRead_HAL(device, pFlcn,
                                    (pQueueInfo->msgQTailBaseAddress +
                                    (pQueue->queuePhyId * pQueueInfo->msgQTailStride)));
    return NV_OK;
}

/*!
 * Set the Message Queue Tail pointer.
 *
 * @param[in]  device  nvswitch device pointer
 * @param[in]  pFlcn   FLCN object pointer
 * @param[in]  pQueue  Pointer to the queue
 * @param[in]  tail    The desired tail value for the message queue
 *
 * @return 'NV_OK' if the tail value was successfully set.
 */
static NV_STATUS
_flcnMsgQueueTailSet_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    FLCNQUEUE       *pQueue,
    NvU32            tail
)
{
    PFALCON_QUEUE_INFO  pQueueInfo = pFlcn->pQueueInfo;

    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueue->queuePhyId < pQueueInfo->msgQTailSize);

    flcnRegWrite_HAL(device, pFlcn,
                            (pQueueInfo->msgQTailBaseAddress +
                            (pQueue->queuePhyId * pQueueInfo->msgQTailStride)),
                            tail);
    return NV_OK;
}

/*!
 * Copies 'sizeBytes' from DMEM offset 'src' to 'pDst' using DMEM access
 * port 'port'.
 *
 * @param[in]   device     nvswitch device pointer
 * @param[in]   pFlcn      FLCN pointer
 * @param[in]   src        The DMEM offset for the source of the copy
 * @param[out]  pDst       Pointer to write with copied data from DMEM
 * @param[in]   sizeBytes  The number of bytes to copy from DMEM
 * @param[in]   port       The DMEM port index to use when accessing the DMEM
 */
static NV_STATUS
_flcnDmemCopyFrom_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            src,
    NvU8            *pDst,
    NvU32            sizeBytes,
    NvU8             port
)
{
    return flcnDmemTransfer_HAL(device, pFlcn,
                                src, pDst, sizeBytes, port,
                                NV_TRUE);                   // bCopyFrom
}

/*!
 * Copies 'sizeBytes' from 'pDst' to DMEM offset 'dst' using DMEM access port
 * 'port'.
 *
 * @param[in]  device     nvswitch device pointer
 * @param[in]  pFlcn      FLCN pointer
 * @param[in]  dst        The destination DMEM offset for the copy
 * @param[in]  pSrc       The pointer to the buffer containing the data to copy
 * @param[in]  sizeBytes  The number of bytes to copy into DMEM
 * @param[in]  port       The DMEM port index to use when accessing the DMEM
 */
static NV_STATUS
_flcnDmemCopyTo_IMPL
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvU8             port
)
{
    return flcnDmemTransfer_HAL(device, pFlcn,
                                dst, pSrc, sizeBytes, port,
                                NV_FALSE);                  // bCopyFrom
}

static void
_flcnPostDiscoveryInit_IMPL
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    flcnableFetchEngines_HAL(device, pFlcn->pFlcnable, &pFlcn->engDescUc, &pFlcn->engDescBc);

    flcnSetupIpHal(device, pFlcn);
}

/* -------------------- Object construction/initialization ------------------- */

/**
 * @brief   set hal object-interface function pointers to flcn implementations
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcn   The flcn for which to set hals
 */
static void
flcnSetupHal
(
    PFLCN pFlcn,
    NvU32 pci_device_id
)
{
    flcn_hal *pHal = NULL;
    if (nvswitch_is_lr10_device_id(pci_device_id))
    {
        flcnSetupHal_LR10(pFlcn);
        goto _flcnSetupHal_success;
    }
    if (nvswitch_is_ls10_device_id(pci_device_id))
    {
        flcnSetupHal_LS10(pFlcn);
        goto _flcnSetupHal_success;
    }

    NVSWITCH_PRINT(NULL, ERROR,
        "Flcn hal can't be setup due to unknown device id\n");
    NVSWITCH_ASSERT(0);

_flcnSetupHal_success:
    //init hal OBJ Interfaces
    pHal = pFlcn->pHal;

    pHal->coreRevisionGet         = _flcnCoreRevisionGet_IMPL;
    pHal->markNotReady            = _flcnMarkNotReady_IMPL;
    pHal->cmdQueueHeadGet         = _flcnCmdQueueHeadGet_IMPL;
    pHal->msgQueueHeadGet         = _flcnMsgQueueHeadGet_IMPL;
    pHal->cmdQueueTailGet         = _flcnCmdQueueTailGet_IMPL;
    pHal->msgQueueTailGet         = _flcnMsgQueueTailGet_IMPL;
    pHal->cmdQueueHeadSet         = _flcnCmdQueueHeadSet_IMPL;
    pHal->msgQueueHeadSet         = _flcnMsgQueueHeadSet_IMPL;
    pHal->cmdQueueTailSet         = _flcnCmdQueueTailSet_IMPL;
    pHal->msgQueueTailSet         = _flcnMsgQueueTailSet_IMPL;

    pHal->dmemCopyFrom            = _flcnDmemCopyFrom_IMPL;
    pHal->dmemCopyTo              = _flcnDmemCopyTo_IMPL;
    pHal->postDiscoveryInit       = _flcnPostDiscoveryInit_IMPL;

    flcnQueueSetupHal(pFlcn);
    flcnRtosSetupHal(pFlcn);
    flcnQueueRdSetupHal(pFlcn);
}

static void
flcnSetupIpHal
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU8 coreRev = flcnableReadCoreRev(device, pFlcn->pFlcnable);

    switch (coreRev) {
        case NV_FLCN_CORE_REV_3_0:
        {
            flcnSetupHal_v03_00(pFlcn);
            break;
        }
        case NV_FLCN_CORE_REV_4_0:
        case NV_FLCN_CORE_REV_4_1:
        {
            flcnSetupHal_v04_00(pFlcn);
            break;
        }
        case NV_FLCN_CORE_REV_5_0:
        case NV_FLCN_CORE_REV_5_1:
        {
            flcnSetupHal_v05_01(pFlcn);
            break;
        }
        case NV_FLCN_CORE_REV_6_0:
        {
            flcnSetupHal_v06_00(pFlcn);
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unsupported falcon core revision: %hhu!\n",
                __FUNCTION__, coreRev);
            NVSWITCH_ASSERT(0);
            break;
        }
    }
}

FLCN *
flcnAllocNew(void)
{
    FLCN *pFlcn = nvswitch_os_malloc(sizeof(*pFlcn));
    if (pFlcn != NULL)
    {
        nvswitch_os_memset(pFlcn, 0, sizeof(*pFlcn));
    }

    return pFlcn;
}

NvlStatus
flcnInit
(
    nvswitch_device    *device,
    FLCN               *pFlcn,
    NvU32               pci_device_id
)
{
    NvlStatus retval = NVL_SUCCESS;

    // allocate hal if a child class hasn't already
    if (pFlcn->pHal == NULL)
    {
        flcn_hal *pHal = pFlcn->pHal = nvswitch_os_malloc(sizeof(*pHal));
        if (pHal == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Flcn allocation failed!\n");
            retval = -NVL_NO_MEM;
            goto flcn_init_fail;
        }
        nvswitch_os_memset(pHal, 0, sizeof(*pHal));
    }

    //don't have a parent class to init, go straight to setupHal
    flcnSetupHal(pFlcn, pci_device_id);

    return retval;

flcn_init_fail:
    flcnDestroy(device, pFlcn);
    return retval;
}

// reverse of flcnInit()
void
flcnDestroy
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    if (pFlcn->pHal != NULL)
    {
        nvswitch_os_free(pFlcn->pHal);
        pFlcn->pHal = NULL;
    }
}
