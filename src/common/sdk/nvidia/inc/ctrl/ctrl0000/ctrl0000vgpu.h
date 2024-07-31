/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0000/ctrl0000vgpu.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrla081.h"
#include "class/cl0000.h"
#include "nv_vgpu_types.h"

/*
 * NV0000_CTRL_CMD_VGPU_CREATE_DEVICE
 *
 * This command informs RM to create a vGPU device on KVM.
 *
 *   vgpuName [IN]
 *     This parameter provides the MDEV UUID or VF BDF depending on whether MDEV
 *     or vfio-pci-core framework is used. 
 *
 *   gpuPciId [IN]
 *     This parameter provides gpuId of GPU on which vgpu device is created.
 *
 *   gpuPciBdf
 *     This parameter specifies the BDF of the VF. (Same as PF for non-sriov)
 *
 *   vgpuTypeId [IN]
 *     This parameter specifies the vGPU type ID for the device to be created.
 *
 *   vgpuId [OUT]
 *     This parameter returns the vgpu id allocated by RM for the device
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_EVENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_CLIENT
 *
 */

#define NV0000_CTRL_CMD_VGPU_CREATE_DEVICE (0xc02) /* finn: Evaluated from "(FINN_NV01_ROOT_VGPU_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_CREATE_DEVICE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_VGPU_CREATE_DEVICE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_VGPU_CREATE_DEVICE_PARAMS {
    NvU8  vgpuName[VM_UUID_SIZE];
    NvU32 gpuPciId;
    NvU32 gpuPciBdf;
    NvU32 vgpuTypeId;
    NvU16 vgpuId;
} NV0000_CTRL_VGPU_CREATE_DEVICE_PARAMS;

/*
 * NV0000_CTRL_CMD_VGPU_GET_INSTANCES
 *
 * This command queries RM for available instances for a particular vGPU type ID
 * on KVM.
 *
 *   gpuPciId [IN]
 *     This parameter specifies gpuId of GPU on which vGPU instances are being
 *     queried.
 *
 *   gpuPciBdf [IN]
 *     This parameter specifies the BDF of the VF. (Same as PF for non-sriov)
 *
 *   numVgpuTypes [IN]
 *     This parameter specifies the count of vgpuTypeIds supplied and the
 *     count of availableInstances values to be returned.
 *
 *   vgpuTypeIds [IN]
 *     This parameter specifies a total of numVgpuTypes vGPU type IDs for which
 *     the available instances are to be queried.
 *
 *   availableInstances [OUT]
 *     This parameter returns a total of numVgpuTypes available instances for
 *     the respective vGPU type IDs supplied in vgpuTypeIds input parameter.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_EVENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_CLIENT
 *   NV_ERR_INVALID_STATE
 *
 */

#define NV0000_CTRL_CMD_VGPU_GET_INSTANCES (0xc03) /* finn: Evaluated from "(FINN_NV01_ROOT_VGPU_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_GET_INSTANCES_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_VGPU_GET_INSTANCES_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_CTRL_VGPU_GET_INSTANCES_PARAMS {
    NvU32 gpuPciId;
    NvU32 gpuPciBdf;
    NvU32 numVgpuTypes;
    NvU32 vgpuTypeIds[NVA081_MAX_VGPU_TYPES_PER_PGPU];
    NvU32 availableInstances[NVA081_MAX_VGPU_TYPES_PER_PGPU];
} NV0000_CTRL_VGPU_GET_INSTANCES_PARAMS;

/*
 * NV0000_CTRL_CMD_VGPU_DELETE_DEVICE
 *
 * This command informs RM to delete a vGPU device on KVM.
 *
 *   vgpuName [IN]
 *     This parameter provides the MDEV UUID or VF BDF depending on whether MDEV
 *     or vfio-pci-core framework is used.
 *
 *   vgpuId [IN]
 *     This parameter provides the vgpu id allocated by RM for the device to be
 *     deleted.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_EVENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_CLIENT
 *
 */

#define NV0000_CTRL_CMD_VGPU_DELETE_DEVICE (0xc04) /* finn: Evaluated from "(FINN_NV01_ROOT_VGPU_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_DELETE_DEVICE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_VGPU_DELETE_DEVICE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_VGPU_DELETE_DEVICE_PARAMS {
    NvU8  vgpuName[VM_UUID_SIZE];
    NvU16 vgpuId;
} NV0000_CTRL_VGPU_DELETE_DEVICE_PARAMS;

/*
 * NV0000_CTRL_CMD_VGPU_VFIO_NOTIFY_RM_STATUS
 *
 * This command informs RM the status of vgpu-vfio GPU operations such as probe and unregister.
 *
 *   returnStatus [IN]
 *     This parameter provides the status of vgpu-vfio GPU operation.
 *
 *   gpuPciId [IN]
 *     This parameter provides the gpu id of the GPU
 */

#define NV0000_CTRL_CMD_VGPU_VFIO_NOTIFY_RM_STATUS (0xc05) /* finn: Evaluated from "(FINN_NV01_ROOT_VGPU_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS {
    NvU32 returnStatus;
    NvU32 gpuId;
} NV0000_CTRL_VGPU_VFIO_NOTIFY_RM_STATUS_PARAMS;

/* _ctrl0000vgpu_h_ */
