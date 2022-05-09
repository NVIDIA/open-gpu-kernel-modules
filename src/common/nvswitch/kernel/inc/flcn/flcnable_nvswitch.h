/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _FLCNABLE_NVSWITCH_H_
#define _FLCNABLE_NVSWITCH_H_

/*!
 * @file   flcnable_nvswitch.h
 * @brief  Provides definitions for all FLCNABLE data structures and interfaces.
 */

#include "flcn/haldefs_flcnable_nvswitch.h"

#include "flcnifcmn.h"

#include "nvlink_errors.h"

struct nvswitch_device;
struct FLCN;
struct FALCON_EXTERNAL_CONFIG;
struct FLCN_QMGR_SEQ_INFO;
union RM_FLCN_MSG;
union RM_FLCN_CMD;
struct ENGINE_DESCRIPTOR_TYPE;

/*!
 * Defines the structure used to contain all generic information related to
 * the FLCNABLE.
 */
typedef struct FLCNABLE
{
    // pointer to our function table - should always be the first thing in any object
    flcnable_hal *pHal;
    // we don't have a parent class, so we go straight to our members

    /* Pointer to FLCN object for the object represented by this FLCNABLE */
    struct FLCN *pFlcn;

} FLCNABLE, *PFLCNABLE;

NvlStatus flcnableInit(struct nvswitch_device *device, PFLCNABLE pFlcnable, NvU32 pci_device_id);
void flcnableDestroy(struct nvswitch_device *device, PFLCNABLE pFlcnable);

/*!
 * Utility to get the FLCN object for the engine
 */
#define ENG_GET_FLCN(pObj) (((PFLCNABLE)pObj)->pFlcn)

/*!
 * Safe (from NULL parent) version of utility to get the FLCN object for the engine
 */
#define ENG_GET_FLCN_IFF(pObj) ((NULL!=(pObj))?ENG_GET_FLCN(pObj):NULL)

// hal functions
NvU8                        flcnableReadCoreRev                         (struct nvswitch_device *device, PFLCNABLE);
void                        flcnableGetExternalConfig                   (struct nvswitch_device *device, PFLCNABLE, struct FALCON_EXTERNAL_CONFIG *);
void                        flcnableEmemCopyFrom                        (struct nvswitch_device *device, PFLCNABLE, NvU32, NvU8 *, NvU32, NvU8);
void                        flcnableEmemCopyTo                          (struct nvswitch_device *device, PFLCNABLE, NvU32, NvU8 *, NvU32, NvU8);
NV_STATUS                   flcnableHandleInitEvent                     (struct nvswitch_device *device, PFLCNABLE, union RM_FLCN_MSG *);
struct FLCN_QMGR_SEQ_INFO * flcnableQueueSeqInfoGet                     (struct nvswitch_device *device, PFLCNABLE, NvU32);
void                        flcnableQueueSeqInfoClear                   (struct nvswitch_device *device, PFLCNABLE, struct FLCN_QMGR_SEQ_INFO *);
void                        flcnableQueueSeqInfoFree                    (struct nvswitch_device *device, PFLCNABLE, struct FLCN_QMGR_SEQ_INFO *);
NvBool                      flcnableQueueCmdValidate                    (struct nvswitch_device *device, PFLCNABLE, union RM_FLCN_CMD *, union RM_FLCN_MSG *, void *, NvU32);
NV_STATUS                   flcnableQueueCmdPostExtension               (struct nvswitch_device *device, PFLCNABLE, union RM_FLCN_CMD *, union RM_FLCN_MSG *, void *, struct NVSWITCH_TIMEOUT *, struct FLCN_QMGR_SEQ_INFO *);
void                        flcnablePostDiscoveryInit                   (struct nvswitch_device *device, PFLCNABLE);

NV_STATUS                   flcnableConstruct_HAL                       (struct nvswitch_device *device, PFLCNABLE);
void                        flcnableDestruct_HAL                        (struct nvswitch_device *device, PFLCNABLE);

void                        flcnableFetchEngines_HAL                    (struct nvswitch_device *device, PFLCNABLE, struct ENGINE_DESCRIPTOR_TYPE *, struct ENGINE_DESCRIPTOR_TYPE *);


#endif // _FLCNABLE_NVSWITCH_H_
