/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"

#include "common_nvswitch.h"
#include "nvstatus.h"

/*!
 * @brief Read the falcon core revision and subversion.
 *
 * @param[in]   device      nvswitch device pointer
 * @param[in]   pFlcnable   FLCNABLE object pointer
 *
 * @return @ref NV_FLCN_CORE_REV_X_Y.
 */
static NvU8
_flcnableReadCoreRev_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable
)
{
    return flcnReadCoreRev_HAL(device, pFlcnable->pFlcn);
}

/*!
 * @brief Get external config
 */
static void
_flcnableGetExternalConfig_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    PFALCON_EXTERNAL_CONFIG pConfig
)
{
    pConfig->bResetInPmc = NV_FALSE;
    pConfig->blkcgBase = 0xffffffff;
    pConfig->fbifBase = 0xffffffff;
}

/*!
 * @brief   Retrieve content from falcon's EMEM.
 */
static void
_flcnableEmemCopyFrom_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            src,
    NvU8            *pDst,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_PRINT(device, ERROR,
        "%s: FLCNABLE interface not implemented on this falcon!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
}

/*!
 * @brief   Write content to falcon's EMEM.
 */
static void
_flcnableEmemCopyTo_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            dst,
    NvU8            *pSrc,
    NvU32            sizeBytes,
    NvU8             port
)
{
    NVSWITCH_PRINT(device, ERROR,
        "%s: FLCNABLE interface not implemented on this falcon!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
}

/*
 * @brief Handle INIT Event
 */
static NV_STATUS
_flcnableHandleInitEvent_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    RM_FLCN_MSG     *pGenMsg
)
{
    return NV_OK;
}

/*!
 * @brief   Retrieves a pointer to the engine specific SEQ_INFO structure.
 *
 * @param[in]   device      nvswitch device pointer
 * @param[in]   pFlcnable   FLCNABLE object pointer
 * @param[in]   seqIndex    Index of the structure to retrieve
 *
 * @return  Pointer to the SEQ_INFO structure or NULL on invalid index.
 */
static PFLCN_QMGR_SEQ_INFO
_flcnableQueueSeqInfoGet_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    NvU32            seqIndex
)
{
    NVSWITCH_PRINT(device, ERROR,
        "%s: FLCNABLE interface not implemented on this falcon!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
    return NULL;
}

/*!
 * @brief   Clear out the engine specific portion of the SEQ_INFO structure.
 *
 * @param[in]   device      nvswitch device pointer
 * @param[in]   pFlcnable   FLCNABLE object pointer
 * @param[in]   pSeqInfo    SEQ_INFO structure pointer
 */
static void
_flcnableQueueSeqInfoClear_IMPL
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
}

/*!
 * @brief   Free up all the engine specific sequence allocations.
 *
 * @param[in]   device      nvswitch device pointer
 * @param[in]   pFlcnable   FLCNABLE object pointer
 * @param[in]   pSeqInfo    SEQ_INFO structure pointer
 */
static void
_flcnableQueueSeqInfoFree_IMPL
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
}

/*!
 * @brief   Validate that the given CMD and related params are properly formed.
 *
 * @copydoc flcnQueueCmdPostNonBlocking_IMPL
 *
 * @return  Boolean if command was properly formed.
 */
static NvBool
_flcnableQueueCmdValidate_IMPL
(
    nvswitch_device *device,
    PFLCNABLE        pFlcnable,
    PRM_FLCN_CMD     pCmd,
    PRM_FLCN_MSG     pMsg,
    void            *pPayload,
    NvU32            queueIdLogical
)
{
    NVSWITCH_PRINT(device, ERROR,
        "%s: FLCNABLE interface not implemented on this falcon!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
    return NV_FALSE;
}

/*!
 * @brief   Engine specific command post actions.
 *
 * @copydoc flcnQueueCmdPostNonBlocking_IMPL
 *
 * @return  NV_OK on success
 *          Failure specific error codes
 */
static NV_STATUS
_flcnableQueueCmdPostExtension_IMPL
(
    nvswitch_device    *device,
    PFLCNABLE           pFlcnable,
    PRM_FLCN_CMD        pCmd,
    PRM_FLCN_MSG        pMsg,
    void               *pPayload,
    NVSWITCH_TIMEOUT   *pTimeout,
    PFLCN_QMGR_SEQ_INFO pSeqInfo
)
{
    return NV_OK;
}

static void
_flcnablePostDiscoveryInit_IMPL
(
    nvswitch_device *device,
    FLCNABLE        *pSoe
)
{
    flcnPostDiscoveryInit(device, pSoe->pFlcn);
}

/**
 * @brief   sets pEngDescUc and pEngDescBc to the discovered
 * engine that matches this flcnable instance
 *
 * @param[in]   device       nvswitch_device pointer
 * @param[in]   pSoe         SOE pointer
 * @param[out]  pEngDescUc  pointer to the UniCast Engine
 *       Descriptor Pointer
 * @param[out]  pEngDescBc  pointer to the BroadCast Engine
 *       Descriptor Pointer
 */
static void
_flcnableFetchEngines_IMPL
(
    nvswitch_device         *device,
    FLCNABLE                *pSoe,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescUc,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescBc
)
{
    // Every falcon REALLY needs to implement this. If they don't flcnRegRead and flcnRegWrite won't work
    NVSWITCH_PRINT(device, ERROR,
        "%s: FLCNABLE interface not implemented on this falcon!\n",
        __FUNCTION__);
    NVSWITCH_ASSERT(0);
}


/* -------------------- Object construction/initialization ------------------- */
static void
flcnableSetupHal
(
    FLCNABLE *pFlcnable,
    NvU32     pci_device_id
)
{
    flcnable_hal *pHal = pFlcnable->pHal;

    //init hal Interfaces
    pHal->readCoreRev                             = _flcnableReadCoreRev_IMPL;
    pHal->getExternalConfig                       = _flcnableGetExternalConfig_IMPL;
    pHal->ememCopyFrom                            = _flcnableEmemCopyFrom_IMPL;
    pHal->ememCopyTo                              = _flcnableEmemCopyTo_IMPL;
    pHal->handleInitEvent                         = _flcnableHandleInitEvent_IMPL;
    pHal->queueSeqInfoGet                         = _flcnableQueueSeqInfoGet_IMPL;
    pHal->queueSeqInfoClear                       = _flcnableQueueSeqInfoClear_IMPL;
    pHal->queueSeqInfoFree                        = _flcnableQueueSeqInfoFree_IMPL;
    pHal->queueCmdValidate                        = _flcnableQueueCmdValidate_IMPL;
    pHal->queueCmdPostExtension                   = _flcnableQueueCmdPostExtension_IMPL;
    pHal->postDiscoveryInit                       = _flcnablePostDiscoveryInit_IMPL;
    pHal->fetchEngines                            = _flcnableFetchEngines_IMPL;
}

NvlStatus
flcnableInit
(
    nvswitch_device    *device,
    FLCNABLE           *pFlcnable,
    NvU32               pci_device_id
)
{
    NvlStatus retval;
    FLCN *pFlcn = NULL;

    // allocate hal if a child class hasn't already
    if (pFlcnable->pHal == NULL)
    {
        flcnable_hal *pHal = pFlcnable->pHal = nvswitch_os_malloc(sizeof(*pHal));
        if (pHal == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Flcn allocation failed!\n");
            retval = -NVL_NO_MEM;
            goto flcnable_init_fail;
        }
        nvswitch_os_memset(pHal, 0, sizeof(*pHal));
    }

    // init flcn - a little out of place here, since we're really only
    // supposed to be initializing hals. However, we need pci_device_id
    // to initialize flcn's hals and flcn is _very_ closely tied to
    // flcnable so it kind of makes some sense to allocate it here
    pFlcn = pFlcnable->pFlcn = flcnAllocNew();
    if (pFlcn == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "Flcn allocation failed!\n");
        retval = -NVL_NO_MEM;
        goto flcnable_init_fail;
    }
    retval = flcnInit(device, pFlcn, pci_device_id);
    if (retval != NVL_SUCCESS)
    {
        goto flcnable_init_fail;
    }

    //don't have a parent class to init, go straight to setupHal
    flcnableSetupHal(pFlcnable, pci_device_id);

    return retval;

flcnable_init_fail:
    flcnableDestroy(device, pFlcnable);
    return retval;
}

// reverse of flcnableInit()
void
flcnableDestroy
(
    nvswitch_device    *device,
    FLCNABLE           *pFlcnable
)
{
    if (pFlcnable->pFlcn != NULL)
    {
        flcnDestroy(device, pFlcnable->pFlcn);
        nvswitch_os_free(pFlcnable->pFlcn);
        pFlcnable->pFlcn = NULL;
    }

    if (pFlcnable->pHal != NULL)
    {
        nvswitch_os_free(pFlcnable->pHal);
        pFlcnable->pHal = NULL;
    }
}
