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
#include "lr10/lr10.h"
#include "flcn/flcn_nvswitch.h"

#include "nvswitch/lr10/dev_falcon_v4.h"

static NvU32
_flcnRegRead_LR10
(
    nvswitch_device *device,
    PFLCN            pFlcn,
    NvU32            offset
)
{
    // Probably should perform some checks on the offset, the device, and the engine descriptor
    return nvswitch_reg_read_32(device, pFlcn->engDescUc.base + offset);
}

static void
_flcnRegWrite_LR10
(
    nvswitch_device    *device,
    PFLCN               pFlcn,
    NvU32               offset,
    NvU32               data
)
{
    // Probably should perform some checks on the offset, the device, and the engine descriptor
    nvswitch_reg_write_32(device, pFlcn->engDescUc.base + offset, data);
}

/*
 * @brief Retrigger an interrupt message from the engine to the NV_CTRL tree
 *
 * @param[in] device  nvswitch_device pointer
 * @param[in] pFlcn   FLCN pointer
 */
static void
_flcnIntrRetrigger_LR10
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    NvU32 val = DRF_DEF(_PFALCON, _FALCON_INTR_RETRIGGER, _TRIGGER, _TRUE);
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_INTR_RETRIGGER(0), val);
}

static NvBool
_flcnAreEngDescsInitialized_LR10
(
    nvswitch_device    *device,
    FLCN               *pFlcn
)
{
    // if pFlcn->engDescUc is 0, we haven't finished discovery, return false
    // if pFlcn->engDescUc is NOT 0, and pFlcn->engDescBc is NULL, this is a unicast only engine
    return   pFlcn->engDescUc.base != 0 && pFlcn->engDescUc.initialized &&
            (pFlcn->engDescBc.base == 0 || pFlcn->engDescBc.initialized);
}

/*
 *  @brief Waits for falcon to finish scrubbing IMEM/DMEM.
 *
 *  @param[in] device   switch device
 *  @param[in] pFlcn    FLCN pointer
 *
 *  @returns nothing
 */
static NV_STATUS
_flcnWaitForResetToFinish_LR10
(
    nvswitch_device    *device,
    PFLCN               pFlcn
)
{
    NVSWITCH_TIMEOUT timeout;
    NvU32 dmaCtrl;

    // Add a dummy write (of anything) to trigger scrubbing
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX0, 0);

    // TODO: Adapt timeout to our model, this should be centralized.
    if (IS_EMULATION(device))
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    }

    while (1)
    {
        dmaCtrl = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_DMACTL);

        if (FLD_TEST_DRF(_PFALCON, _FALCON_DMACTL, _DMEM_SCRUBBING, _DONE, dmaCtrl) &&
            FLD_TEST_DRF(_PFALCON, _FALCON_DMACTL, _IMEM_SCRUBBING, _DONE, dmaCtrl))
        {
            // Operation successful, IMEM and DMEM scrubbing has finished.
            return NV_OK;                    
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for scrubbing to finish!!!\n",
                __FUNCTION__);
            NVSWITCH_ASSERT(0);
            return NV_ERR_TIMEOUT;
        }
    }
}

/*!
 * @brief   Capture and dump the falconPC trace.
 *
 * @param[in]  device     nvswitch device pointer
 * @param[in]  pFlcn      FLCN object pointer
 *
 * @returns nothing
 */
void
_flcnDbgInfoCapturePcTrace_LR10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU32    regTraceIdx;
    NvU32    idx;
    NvU32    maxIdx;

    // Dump entire PC trace buffer
    regTraceIdx = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_TRACEIDX);
    maxIdx      = DRF_VAL(_PFALCON_FALCON, _TRACEIDX, _MAXIDX, regTraceIdx);

    NVSWITCH_PRINT(device, ERROR,
              "PC TRACE (TOTAL %d ENTRIES. Entry 0 is the most recent branch):\n",
              maxIdx);

    for (idx = 0; idx < maxIdx; idx++)
    {
        regTraceIdx =
            FLD_SET_DRF_NUM(_PFALCON, _FALCON_TRACEIDX, _IDX, idx, regTraceIdx);

        flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_TRACEIDX, regTraceIdx);

        NVSWITCH_PRINT(device, ERROR, "FALCON_TRACEPC(%d)     : 0x%08x\n", idx,
            DRF_VAL(_PFALCON, _FALCON_TRACEPC, _PC,
                flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_TRACEPC)));
    }
}

/*!
 * @brief Read falcon core revision
 *
 * @param[in] device nvswitch_device pointer
 * @param[in] pFlcn  FLCN pointer
 *
 * @return @ref NV_FLCN_CORE_REV_X_Y.
 */
NvU8
_flcnReadCoreRev_LR10
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU32 hwcfg1 = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_HWCFG1);

    return ((DRF_VAL(_PFALCON, _FALCON_HWCFG1, _CORE_REV, hwcfg1) << 4) |
            DRF_VAL(_PFALCON, _FALCON_HWCFG1, _CORE_REV_SUBVERSION, hwcfg1));
}

//
// Store pointers to ucode header and data.
// Preload ucode from registry if available.
//
NV_STATUS
_flcnConstruct_LR10
(
    nvswitch_device    *device,
    PFLCN               pFlcn
)
{
    NV_STATUS          status;
    PFLCNABLE          pFlcnable = pFlcn->pFlcnable;
    PFALCON_QUEUE_INFO pQueueInfo;
    pFlcn->bConstructed         = NV_TRUE;

    // Set the arch to Falcon
    pFlcn->engArch = NV_UPROC_ENGINE_ARCH_FALCON;

    // Allocate the memory for Queue Data Structure if needed.
    if (pFlcn->bQueuesEnabled)
    {
        pQueueInfo = pFlcn->pQueueInfo = nvswitch_os_malloc(sizeof(*pQueueInfo));
        if (pQueueInfo == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NVSWITCH_ASSERT(0);
            goto _flcnConstruct_LR10_fail;
        }
        nvswitch_os_memset(pQueueInfo, 0, sizeof(FALCON_QUEUE_INFO));
        // Assert if Number of Queues are zero
        NVSWITCH_ASSERT(pFlcn->numQueues != 0);
        pQueueInfo->pQueues = nvswitch_os_malloc(sizeof(FLCNQUEUE) * pFlcn->numQueues);
        if (pQueueInfo->pQueues == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            NVSWITCH_ASSERT(0);
            goto _flcnConstruct_LR10_fail;
        }
        nvswitch_os_memset(pQueueInfo->pQueues, 0, sizeof(FLCNQUEUE) * pFlcn->numQueues);
        // Sequences can be optional
        if (pFlcn->numSequences != 0)
        {
            if ((pFlcn->numSequences - 1) > ((NvU32)NV_U8_MAX))
            {
                status = NV_ERR_OUT_OF_RANGE;
                NVSWITCH_PRINT(device, ERROR,
                          "Max numSequences index = %d cannot fit into byte\n",
                          (pFlcn->numSequences - 1));
                NVSWITCH_ASSERT(0);
                goto _flcnConstruct_LR10_fail;
            }
            flcnQueueSeqInfoStateInit(device, pFlcn);
        }
    }
    // DEBUG
    NVSWITCH_PRINT(device, INFO, "Falcon: %s\n", flcnGetName_HAL(device, pFlcn));
    NVSWITCH_ASSERT(pFlcnable != NULL);
    flcnableGetExternalConfig(device, pFlcnable, &pFlcn->extConfig);
    return NV_OK;
_flcnConstruct_LR10_fail:
    // call flcnDestruct to free the memory allocated in this construct function
    flcnDestruct_HAL(device, pFlcn);
    return status;
}

void
_flcnDestruct_LR10
(
    nvswitch_device    *device,
    PFLCN               pFlcn
)
{
    PFALCON_QUEUE_INFO pQueueInfo;
    PFLCNABLE pFlcnable = pFlcn->pFlcnable;
    if (!pFlcn->bConstructed)
    {
        return;
    }
    pFlcn->bConstructed = NV_FALSE;
    if (pFlcnable == NULL) {
        NVSWITCH_ASSERT(pFlcnable != NULL);
        return;
    }
    if (pFlcn->bQueuesEnabled && (pFlcn->pQueueInfo != NULL))
    {
        pQueueInfo = pFlcn->pQueueInfo;
        if (NULL != pQueueInfo->pQueues)
        {
            nvswitch_os_free(pQueueInfo->pQueues);
            pQueueInfo->pQueues = NULL;
        }
        nvswitch_os_free(pFlcn->pQueueInfo);
        pFlcn->pQueueInfo = NULL;
    }
}
const char *
_flcnGetName_LR10
(
    nvswitch_device    *device,
    PFLCN               pFlcn
)
{
    if (pFlcn->name == NULL)
    {
        return "UNKNOWN";
    }
    return pFlcn->name;
}

/**
 * @brief   set hal function pointers for functions defined in LR10 (i.e. this file)
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcn   The flcn for which to set hals
 */
void
flcnSetupHal_LR10
(
    PFLCN            pFlcn
)
{
    flcn_hal *pHal = pFlcn->pHal;

    pHal->readCoreRev              = _flcnReadCoreRev_LR10;
    pHal->regRead                  = _flcnRegRead_LR10;
    pHal->regWrite                 = _flcnRegWrite_LR10;
    pHal->construct                = _flcnConstruct_LR10;
    pHal->destruct                 = _flcnDestruct_LR10;
    pHal->getName                  = _flcnGetName_LR10;
    pHal->intrRetrigger            = _flcnIntrRetrigger_LR10;
    pHal->areEngDescsInitialized   = _flcnAreEngDescsInitialized_LR10;
    pHal->waitForResetToFinish     = _flcnWaitForResetToFinish_LR10;
    pHal->dbgInfoCapturePcTrace    = _flcnDbgInfoCapturePcTrace_LR10;
}
