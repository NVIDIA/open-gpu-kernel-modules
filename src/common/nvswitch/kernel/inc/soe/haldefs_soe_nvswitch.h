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

#ifndef _HALDEFS_SOE_NVSWITCH_H_
#define _HALDEFS_SOE_NVSWITCH_H_


#include "nvstatus.h"
#include "flcnifcmn.h"
#include "flcn/haldefs_flcnable_nvswitch.h"
#include "common_nvswitch.h"

struct SOE;

typedef struct {
    // needs to be the first thing in this struct so that a soe_hal* can be
    // re-interpreted as a flcnable_hal* and vise-versa.
    flcnable_hal base;

    //add any hal functions specific to SOE here
    NV_STATUS                   (*processMessages)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NV_STATUS                   (*waitForInitAck)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);

    NvU32                       (*service)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    void                        (*serviceHalt)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    void                        (*ememTransfer)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe,
                                        NvU32                           dmemAddr,
                                        NvU8                           *pBuf,
                                        NvU32                           sizeBytes,
                                        NvU8                            port,
                                        NvBool                          bCopyFrom);
    NvU32                       (*getEmemSize)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NvU32                       (*getEmemStartOffset)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NV_STATUS                   (*ememPortToRegAddr)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe,
                                        NvU32                           port,
                                        NvU32                          *pEmemCAddr,
                                        NvU32                          *pEmemDAddr);
    void                        (*serviceExterr)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NV_STATUS                   (*getExtErrRegAddrs)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe,
                                        NvU32                          *pExtErrAddr,
                                        NvU32                          *pExtErrStat);
    NvU32                       (*ememPortSizeGet)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NvBool                      (*isCpuHalted)(
                                        struct nvswitch_device         *device,
                                        struct SOE                     *pSoe);
    NvlStatus                    (*testDma)(
                                        struct nvswitch_device         *device);
    NvlStatus                   (*setPexEOM)(
                                        struct nvswitch_device         *device,
                                        NvU8                            mode, 
                                        NvU8                            nblks,
                                        NvU8                            nerrs,
                                        NvU8                            berEyeSel);
    NvlStatus                   (*getPexEomStatus)(
                                        struct nvswitch_device         *device,
                                        NvU8                            mode,
                                        NvU8                            nblks,
                                        NvU8                            nerrs,
                                        NvU8                            berEyeSel,
                                        NvU32                           laneMask,
                                        NvU16                          *pEomStatus);
    NvlStatus                   (*getUphyDlnCfgSpace)(
                                        struct nvswitch_device         *device,
                                        NvU32                           regAddress,
                                        NvU32                           laneSelectMask,
                                        NvU16                          *pRegValue);
    NvlStatus                   (*forceThermalSlowdown)(
                                        struct nvswitch_device         *device,
                                        NvBool                          slowdown,
                                        NvU32                           periodUs);
    NvlStatus                   (*setPcieLinkSpeed)(
                                        struct nvswitch_device         *device,
                                        NvU32                           linkSpeed);
    NvlStatus                   (*i2cAccess)(
                                        struct nvswitch_device           *device,
                                        NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams);
} soe_hal;

// HAL functions
void soeSetupHal_LR10(struct SOE *pSoe);
void soeSetupHal_LS10(struct SOE *pSoe);

#endif //_HALDEFS_SOE_NVSWITCH_H_
