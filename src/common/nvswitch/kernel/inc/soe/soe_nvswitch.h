/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOE_NVSWITCH_H_
#define _SOE_NVSWITCH_H_

#include "nvlink_errors.h"
#include "nvtypes.h"
#include "nvstatus.h"
#include "common_nvswitch.h"

typedef struct SOE SOE, *PSOE;
struct FLCNABLE;
struct nvswitch_device;

SOE *soeAllocNew(void);
NvlStatus soeInit(struct nvswitch_device *device, PSOE pSoe, NvU32 pci_device_id);
void soeDestroy(struct nvswitch_device *device, PSOE pSoe);

//HAL functions
NV_STATUS   soeProcessMessages_HAL      (struct nvswitch_device *device, PSOE pSoe);
NV_STATUS   soeWaitForInitAck_HAL       (struct nvswitch_device *device, PSOE pSoe);
NvU32       soeService_HAL              (struct nvswitch_device *device, PSOE pSoe);
void        soeServiceHalt_HAL          (struct nvswitch_device *device, PSOE pSoe);
void        soeEmemTransfer_HAL         (struct nvswitch_device *device, PSOE pSoe, NvU32 dmemAddr, NvU8 *pBuf, NvU32 sizeBytes, NvU8 port, NvBool bCopyFrom);
NvU32       soeGetEmemSize_HAL          (struct nvswitch_device *device, PSOE pSoe);
NvU32       soeGetEmemStartOffset_HAL   (struct nvswitch_device *device, PSOE pSoe);
NV_STATUS   soeEmemPortToRegAddr_HAL    (struct nvswitch_device *device, PSOE pSoe, NvU32 port, NvU32 *pEmemCAddr, NvU32 *pEmemDAddr);
void        soeServiceExterr_HAL        (struct nvswitch_device *device, PSOE pSoe);
NV_STATUS   soeGetExtErrRegAddrs_HAL    (struct nvswitch_device *device, PSOE pSoe, NvU32 *pExtErrAddr, NvU32 *pExtErrStat);
NvU32       soeEmemPortSizeGet_HAL      (struct nvswitch_device *device, PSOE pSoe);
NvBool      soeIsCpuHalted_HAL          (struct nvswitch_device *device, PSOE pSoe);
NvlStatus   soeTestDma_HAL              (struct nvswitch_device *device, PSOE pSoe);
NvlStatus   soeSetPexEOM_HAL            (struct nvswitch_device *device, NvU8 mode, NvU8 nblks, NvU8 nerrs, NvU8 berEyeSel);
NvlStatus   soeGetPexEomStatus_HAL      (struct nvswitch_device *device, NvU8 mode, NvU8 nblks, NvU8 nerrs, NvU8 berEyeSel, NvU32 laneMask, NvU16  *pEomStatus);
NvlStatus   soeGetUphyDlnCfgSpace_HAL   (struct nvswitch_device *device, NvU32 regAddress, NvU32 laneSelectMask, NvU16 *pRegValue);
NvlStatus   soeForceThermalSlowdown_HAL (struct nvswitch_device *device, NvBool slowdown, NvU32  periodUs);
NvlStatus   soeSetPcieLinkSpeed_HAL     (struct nvswitch_device *device, NvU32 linkSpeed);
NvlStatus   soeI2CAccess_HAL            (struct nvswitch_device *device, NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams);

#endif //_SOE_NVSWITCH_H_
