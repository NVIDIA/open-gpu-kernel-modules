/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cla084.finn
//

#include "nv_vgpu_types.h"
#include "cla084_notification.h"

#define NVA084_KERNEL_HOST_VGPU_DEVICE (0xa084U) /* finn: Evaluated from "NVA084_ALLOC_PARAMETERS_MESSAGE_ID" */

#define NVA084_MAX_VMMU_SEGMENTS       384

/*
 * NVA084_ALLOC_PARAMETERS
 *
 * This structure represents vGPU host device KERNEL object allocation parameters.
 * dbdf -> domain (31:16), bus (15:8), device (7:3), function (2:0)
 * gfid -> Used only when SRIOV is enabled otherwise set to 0.
 * swizzId [IN] -> Used only when MIG mode is enabled otherwise set
 *                     to NV2080_CTRL_GPU_PARTITION_ID_INVALID.
 * numChannels -> Used only when SRIOV is enabled. Must be a power of 2.
 * placementId [IN] -> Used to provide placement ID of with heterogeneous timesliced vGPUs.
 *                     Otherwise set to NVA081_PLACEMENT_ID_INVALID.
 * bDisableDefaultSmcExecPartRestore - If set to true, SMC default execution partition
 *                                     save/restore will not be done in host-RM
 * vgpuDeviceInstanceId -> Specifies the vGPU device instance per VM to be used
 *                         for supporting multiple vGPUs per VM.
 * hPluginClient -> handle to the plugin client
 * numGuestFbHandles -> number of guest memory handles, the client handle is hPluginClient
 * guestFbHandleList -> handle list to guest memory
 * hPluginHeapMemory -> plugin heap memory handle, the client handle is hPluginClient
 * hMigRmHeapMemory -> MIG-RM heap memory handle
 * bDeviceProfilingEnabled -> If set to true, profiling is allowed
 * bGpupLiveMigrationEnabled -> True if GPUP LM is supported
 */
#define NVA084_ALLOC_PARAMETERS_MESSAGE_ID (0xa084U)

typedef struct NVA084_ALLOC_PARAMETERS {
    NvU32      dbdf;
    NvU32      gfid;
    NvU32      swizzId;
    NvU32      vgpuType;
    NvU32      vmPid;
    NvU32      numChannels;
    NvU32      numPluginChannels;
    NvU16      placementId;
    VM_ID_TYPE vmIdType;
    NV_DECLARE_ALIGNED(VM_ID guestVmId, 8);
    NvBool     bDisableDefaultSmcExecPartRestore;
    NvU32      vgpuDeviceInstanceId;
    NvHandle   hPluginClient;
    NvU8       vgpuDevName[VM_UUID_SIZE];
    NvBool     bGpupLiveMigrationEnabled;
} NVA084_ALLOC_PARAMETERS;
