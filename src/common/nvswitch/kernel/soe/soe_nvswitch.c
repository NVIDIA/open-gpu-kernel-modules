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

#include "soe/soe_nvswitch.h"
#include "soe/soe_priv_nvswitch.h"

#include "flcn/haldefs_flcnable_nvswitch.h"
#include "flcn/haldefs_flcn_nvswitch.h"
#include "flcn/flcn_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"
#include "common_nvswitch.h"

/*!
 * Copies 'sizeBytes' from DMEM address 'src' to 'pDst' using EMEM access port.
 *
 * The address must be located in the EMEM region located directly above the
 * maximum virtual address of DMEM.
 *
 * @param[in]   device      nvswitch_device pointer
 * @param[in]   pSoe        SOE pointer
 * @param[in]   src         The DMEM address for the source of the copy
 * @param[out]  pDst        Pointer to write with copied data from EMEM
 * @param[in]   sizeBytes   The number of bytes to copy from EMEM
 * @param[in]   port        EMEM port
 */
static void
_soeEmemCopyFrom_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               src,
    NvU8               *pDst,
    NvU32               sizeBytes,
    NvU8                port
)
{
    soeEmemTransfer_HAL(device, (PSOE)pSoe, src, pDst, sizeBytes, port, NV_TRUE);
}

/*!
 * Copies 'sizeBytes' from 'pDst' to DMEM address 'dst' using EMEM access port.
 *
 * The address must be located in the EMEM region located directly above the
 * maximum virtual address of DMEM.
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 * @param[in]  dst         The DMEM address for the copy destination.
 * @param[in]  pSrc        The pointer to the buffer containing the data to copy
 * @param[in]  sizeBytes   The number of bytes to copy into EMEM
 * @param[in]  port        EMEM port
 */
static void
_soeEmemCopyTo_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               dst,
    NvU8               *pSrc,
    NvU32               sizeBytes,
    NvU8                port
)
{
    soeEmemTransfer_HAL(device, (PSOE)pSoe, dst, pSrc, sizeBytes, port, NV_FALSE);
}

/*!
 * @brief   Retrieves a pointer to the engine specific SEQ_INFO structure.
 *
 * @param[in]   device      nvswitch_device pointer
 * @param[in]   pSoe        SOE pointer
 * @param[in]   seqIndex    Index of the structure to retrieve
 *
 * @return  Pointer to the SEQ_INFO structure or NULL on invalid index.
 */
static PFLCN_QMGR_SEQ_INFO
_soeQueueSeqInfoGet_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    NvU32               seqIndex
)
{
    FLCN *pFlcn = ENG_GET_FLCN(pSoe);

    if (seqIndex < pFlcn->numSequences)
    {
        return &(((PSOE)pSoe)->seqInfo[seqIndex]);
    }
    return NULL;
}

/*!
 * @copydoc flcnableQueueCmdValidate_IMPL
 */
static NvBool
_soeQueueCmdValidate_IMPL
(
    nvswitch_device    *device,
    FLCNABLE           *pSoe,
    PRM_FLCN_CMD        pCmd,
    PRM_FLCN_MSG        pMsg,
    void               *pPayload,
    NvU32               queueIdLogical
)
{
    PFLCN       pFlcn   = ENG_GET_FLCN(pSoe);
    FLCNQUEUE  *pQueue  = &pFlcn->pQueueInfo->pQueues[queueIdLogical];
    NvU32       cmdSize = pCmd->cmdGen.hdr.size;

    // Verify that the target queue ID represents a valid RM queue.
    if (queueIdLogical != SOE_RM_CMDQ_LOG_ID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid SOE command queue ID = 0x%x\n",
            __FUNCTION__, queueIdLogical);
        return NV_FALSE;
    }

    //
    // Command size cannot be larger than queue size / 2. Otherwise, it is
    // impossible to send two commands back to back if we start from the
    // beginning of the queue.
    //
    if (cmdSize > (pQueue->queueSize / 2))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid command (illegal size = 0x%x)\n",
            __FUNCTION__, cmdSize);
        return NV_FALSE;
    }

    // Validate the command's unit identifier.
    if (!RM_SOE_UNITID_IS_VALID(pCmd->cmdGen.hdr.unitId))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid unitID = %d\n",
            __FUNCTION__, pCmd->cmdGen.hdr.unitId);
        return NV_FALSE;
    }

    return NV_TRUE;
}

/* -------------------- Object construction/initialization ------------------- */

static void
soeSetupHal
(
    SOE      *pSoe,
    NvU32     pci_device_id
)
{
    soe_hal *pHal = NULL;
    flcnable_hal *pParentHal = NULL;

    if (nvswitch_is_lr10_device_id(pci_device_id))
    {
        soeSetupHal_LR10(pSoe);
    }
    else if (nvswitch_is_ls10_device_id(pci_device_id))
    {
        soeSetupHal_LS10(pSoe);
    }
    else
    {
        // we're on a device which doesn't support SOE
        NVSWITCH_PRINT(NULL, ERROR, "Tried to initialize SOE on device with no SOE\n");
        NVSWITCH_ASSERT(0);
    }

    pHal = pSoe->base.pHal;
    pParentHal = (flcnable_hal *)pHal;
    //set any functions we want to override
    pParentHal->ememCopyTo          = _soeEmemCopyTo_IMPL;
    pParentHal->ememCopyFrom        = _soeEmemCopyFrom_IMPL;
    pParentHal->queueSeqInfoGet     = _soeQueueSeqInfoGet_IMPL;
    pParentHal->queueCmdValidate    = _soeQueueCmdValidate_IMPL;
}

SOE *
soeAllocNew(void)
{
    SOE *pSoe = nvswitch_os_malloc(sizeof(*pSoe));
    if (pSoe != NULL)
    {
        nvswitch_os_memset(pSoe, 0, sizeof(*pSoe));
    }

    return pSoe;
}

NvlStatus
soeInit
(
    nvswitch_device    *device,
    SOE                *pSoe,
    NvU32               pci_device_id
)
{
    NvlStatus retval;

    // allocate hal if a child class hasn't already
    if (pSoe->base.pHal == NULL)
    {
        soe_hal *pHal = pSoe->base.pHal = nvswitch_os_malloc(sizeof(*pHal));
        if (pHal == NULL)
        {
            NVSWITCH_PRINT(device, ERROR, "Flcn allocation failed!\n");
            retval = -NVL_NO_MEM;
            goto soe_init_fail;
        }
        nvswitch_os_memset(pHal, 0, sizeof(*pHal));
    }

    // init parent class
    retval = flcnableInit(device, (PFLCNABLE)pSoe, pci_device_id);
    if (retval != NVL_SUCCESS)
    {
        goto soe_init_fail;
    }

    soeSetupHal(pSoe, pci_device_id);

    return retval;
soe_init_fail:
    soeDestroy(device, pSoe);
    return retval;
}

// reverse of soeInit()
void
soeDestroy
(
    nvswitch_device    *device,
    SOE                *pSoe
)
{
    // destroy parent class
    flcnableDestroy(device, (PFLCNABLE)pSoe);

    if (pSoe->base.pHal != NULL)
    {
        nvswitch_os_free(pSoe->base.pHal);
        pSoe->base.pHal = NULL;
    }
}
