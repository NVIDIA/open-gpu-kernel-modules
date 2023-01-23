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

#include "common_nvswitch.h"
#include "soe/haldefs_soe_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "soe/soe_priv_nvswitch.h"

#include "export_nvswitch.h"

NV_STATUS
soeProcessMessages
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->processMessages == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pSoe->base.pHal->processMessages(device, pSoe);
}

NV_STATUS
soeWaitForInitAck
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->waitForInitAck == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pSoe->base.pHal->waitForInitAck(device, pSoe);
}



NvU32
soeService_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->service == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->service(device, pSoe);
}

void
soeServiceHalt_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->serviceHalt == NULL)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pSoe->base.pHal->serviceHalt(device, pSoe);
}

void
soeEmemTransfer_HAL
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32            dmemAddr,
    NvU8            *pBuf,
    NvU32            sizeBytes,
    NvU8             port,
    NvBool           bCopyFrom
)
{
    if (pSoe->base.pHal->ememTransfer == NULL)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pSoe->base.pHal->ememTransfer(device, pSoe, dmemAddr, pBuf, sizeBytes, port, bCopyFrom);
}

NvU32
soeGetEmemSize_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->getEmemSize == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->getEmemSize(device, pSoe);
}

NvU32
soeGetEmemStartOffset_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->getEmemStartOffset == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->getEmemStartOffset(device, pSoe);
}

NV_STATUS
soeEmemPortToRegAddr_HAL
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32            port,
    NvU32           *pEmemCAddr,
    NvU32           *pEmemDAddr
)
{
    if (pSoe->base.pHal->ememPortToRegAddr == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pSoe->base.pHal->ememPortToRegAddr(device, pSoe, port, pEmemCAddr, pEmemDAddr);
}

void
soeServiceExterr_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->serviceExterr == NULL)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    pSoe->base.pHal->serviceExterr(device, pSoe);
}

NV_STATUS
soeGetExtErrRegAddrs_HAL
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32           *pExtErrAddr,
    NvU32           *pExtErrStat
)
{
    if (pSoe->base.pHal->getExtErrRegAddrs == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return pSoe->base.pHal->getExtErrRegAddrs(device, pSoe, pExtErrAddr, pExtErrStat);
}

NvU32
soeEmemPortSizeGet_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->ememPortSizeGet == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->ememPortSizeGet(device, pSoe);
}

NvBool
soeIsCpuHalted_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->isCpuHalted == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_FALSE;
    }

    return pSoe->base.pHal->isCpuHalted(device, pSoe);
}

NvlStatus
soeTestDma_HAL
(
    nvswitch_device *device,
    PSOE            pSoe
)
{
    if (pSoe->base.pHal->testDma == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->testDma(device);
}

NvlStatus
soeSetPexEOM_HAL
(
    nvswitch_device *device,
    NvU8 mode,
    NvU8 nblks,
    NvU8 nerrs,
    NvU8 berEyeSel
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->setPexEOM == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->setPexEOM(device, mode, nblks, nerrs, berEyeSel);
}

NvlStatus
soeGetPexEomStatus_HAL
(
    nvswitch_device *device,
    NvU8 mode,
    NvU8 nblks,
    NvU8 nerrs,
    NvU8 berEyeSel,
    NvU32 laneMask,
    NvU16 *pEomStatus
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->getPexEomStatus == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->getPexEomStatus(device, mode, nblks, nerrs, berEyeSel, laneMask, pEomStatus);
}

NvlStatus
soeGetUphyDlnCfgSpace_HAL
(
    nvswitch_device *device,
    NvU32 regAddress,
    NvU32 laneSelectMask,
    NvU16 *pRegValue
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->getUphyDlnCfgSpace == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->getUphyDlnCfgSpace(device, regAddress, laneSelectMask, pRegValue);
}

NvlStatus
soeForceThermalSlowdown_HAL
(
    nvswitch_device *device,
    NvBool slowdown,
    NvU32  periodUs
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->forceThermalSlowdown == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->forceThermalSlowdown(device, slowdown, periodUs);
}

NvlStatus
soeSetPcieLinkSpeed_HAL
(
    nvswitch_device *device,
    NvU32 linkSpeed
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->setPcieLinkSpeed == NULL)
    {
        NVSWITCH_ASSERT(0);
        return -NVL_BAD_ARGS;
    }

    return pSoe->base.pHal->setPcieLinkSpeed(device, linkSpeed);
}

NV_STATUS
soeProcessMessages_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->processMessages == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->processMessages(device, pSoe);
}

NV_STATUS
soeWaitForInitAck_HAL
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    if (pSoe->base.pHal->waitForInitAck == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->waitForInitAck(device, pSoe);
}

NvlStatus
soeI2CAccess_HAL
(
    nvswitch_device *device,
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams
)
{
    PSOE pSoe = (PSOE)device->pSoe;
    if (pSoe->base.pHal->i2cAccess == NULL)
    {
        NVSWITCH_ASSERT(0);
        return 0;
    }

    return pSoe->base.pHal->i2cAccess(device, pParams);
}
