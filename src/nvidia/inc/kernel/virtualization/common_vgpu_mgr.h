/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __common_vgpu_mgr_h__
#define __common_vgpu_mgr_h__

#include "gpu/gpu.h"
#include "gpu/fifo/kernel_fifo.h"
#include "ctrl/ctrla081.h"

#include "containers/list.h"

#define MAX_VGPU_TYPES_PER_PGPU                        NVA081_MAX_VGPU_TYPES_PER_PGPU
#define VGPU_CONFIG_PARAMS_MAX_LENGTH                  1024
#define VGPU_STRING_BUFFER_SIZE                        NVA081_VGPU_STRING_BUFFER_SIZE
#define VGPU_UUID_SIZE                                 NVA081_VM_UUID_SIZE
#define VGPU_MAX_GFID                                  64
#define VGPU_SIGNATURE_SIZE                            NVA081_VGPU_SIGNATURE_SIZE
#define VGPU_MAX_PLUGIN_CHANNELS                       5
#define MAX_VGPU_DEVICES_PER_PGPU                      NVA081_MAX_VGPU_PER_PGPU
#define MAX_VGPU_DEVICES_PER_GI                        NVA081_MAX_VGPU_PER_GI

#define SET_GUEST_ID_ACTION_SET                        0
#define SET_GUEST_ID_ACTION_UNSET                      1

// swrl count for MIG when running in non-timesliced mode
#define OBJSCHED_SW_MIG_NO_TIMESLICE_RUNLIST_COUNT     1
// swrl count for MIG when running in timesliced mode
#define OBJSCHED_SW_MIG_TIMESLICE_RUNLIST_COUNT        13
// swrl count for non-mig
#define OBJSCHED_SW_RUNLIST_COUNT                      33


typedef struct
{
    NvU8 action;
    NvU32 vmPid;
    VM_ID_TYPE vmIdType;
    VM_ID guestVmId;
} SET_GUEST_ID_PARAMS;

/* This structure stores per vGPU instance supported placement information */
typedef struct
{
    /* For Heterogeneous vGPU mode only */
    NvU16       heterogeneousSupportedPlacementId;
    NvU16       heterogeneousSupportedChidOffset;
    /* For Homogeneous vGPU placement mode only */
    NvU16       homogeneousSupportedPlacementId;
    NvU16       homogeneousSupportedChidOffset;
} VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO;

/* This structure stores per vGPU type's placement information */
typedef struct
{
    NvU32                                   placementSize;
    NvU32                                   channelCount;
    VGPU_INSTANCE_SUPPORTED_PLACEMENT_INFO  vgpuInstanceSupportedPlacementInfo[MAX_VGPU_DEVICES_PER_PGPU];
    NvU16                                   heterogeneousPlacementCount;
    NvU16                                   homogeneousPlacementCount;
} VGPU_TYPE_SUPPORTED_PLACEMENT_INFO;

/* This structure represents the vGPU type's attributes */
typedef struct
{
    NvU32                               vgpuTypeId;
    NvU8                                vgpuName[VGPU_STRING_BUFFER_SIZE];
    NvU8                                vgpuClass[VGPU_STRING_BUFFER_SIZE];
    NvU8                                license[NV_GRID_LICENSE_INFO_MAX_LENGTH];
    NvU8                                licensedProductName[NV_GRID_LICENSE_INFO_MAX_LENGTH];
    VGPU_TYPE_SUPPORTED_PLACEMENT_INFO  vgpuTypeSupportedPlacementInfo;
    NvU32                               maxInstance;
    NvU32                               numHeads;
    NvU32                               maxResolutionX;
    NvU32                               maxResolutionY;
    NvU32                               maxPixels;
    NvU32                               frlConfig;
    NvU32                               cudaEnabled;
    NvU32                               eccSupported;
    NvU32                               gpuInstanceSize;
    NvU32                               multiVgpuSupported;
    NvU64                               vdevId NV_ALIGN_BYTES(8);
    NvU64                               pdevId NV_ALIGN_BYTES(8);
    NvU64                               profileSize NV_ALIGN_BYTES(8);
    NvU64                               fbLength NV_ALIGN_BYTES(8);
    NvU64                               gspHeapSize NV_ALIGN_BYTES(8);
    NvU64                               fbReservation NV_ALIGN_BYTES(8);
    NvU64                               mappableVideoSize NV_ALIGN_BYTES(8);
    NvU32                               encoderCapacity;
    NvU64                               bar1Length NV_ALIGN_BYTES(8);
    NvU32                               frlEnable;
    NvU32                               gpuDirectSupported;
    NvU32                               nvlinkP2PSupported;
    NvU32                               maxInstancePerGI;
    NvU32                               multiVgpuExclusive;
    NvU8                                vgpuExtraParams[VGPU_CONFIG_PARAMS_MAX_LENGTH];
    NvU8                                vgpuSignature[VGPU_SIGNATURE_SIZE];
} VGPU_TYPE;

MAKE_LIST(VGPU_TYPE_LIST, VGPU_TYPE);

void
vgpuMgrFillVgpuType(NVA081_CTRL_VGPU_INFO *pVgpuInfo, VGPU_TYPE *pVgpuTypeNode);

NV_STATUS
vgpuMgrReserveSystemChannelIDs(OBJGPU *pGpu,
                               VGPU_TYPE *vgpuTypeInfo,
                               NvU32 gfid,
                               NvU32 *pChidOffset,
                               NvU32 *pChannelCount,
                               Device *pMigDevice,
                               NvU32 numChannels,
                               NvU16 placementId,
                               NvU32 engineFifoListNumEntries,
                               FIFO_ENGINE_LIST *engineFifoList);

void
vgpuMgrFreeSystemChannelIDs(OBJGPU *pGpu,
                            NvU32 gfid,
                            NvU32 *pChidOffset,
                            NvU32 *pChannelCount,
                            Device *pMigDevice,
                            NvU32 engineFifoListNumEntries,
                            FIFO_ENGINE_LIST *engineFifoList);


NvU32 vgpuMgrGetSwrlCountToAllocate(OBJGPU *pGpu);

#endif // __common_vgpu_mgr_h__
