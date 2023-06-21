/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * NV0000_CTRL_CMD_VGPU_GET_START_DATA
 *
 * This command gets data associated with NV0000_NOTIFIERS_VGPU_MGR_START to
 * start VGPU process.
 *
 *   mdevUuid
 *     This parameter gives mdev device UUID for which nvidia-vgpu-mgr should
 *     init process.
 *
 *   qemuPid
 *     This parameter specifies the QEMU process ID of the VM.
 *
 *   gpuPciId
 *     This parameter provides gpuId of GPU on which vgpu device is created.
 *
 *   configParams
 *     This parameter specifies the configuration parameters for vGPU
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_EVENT
 *   NV_ERR_OBJECT_NOT_FOUND
 *   NV_ERR_INVALID_CLIENT
 *
 */
#define NV0000_CTRL_CMD_VGPU_GET_START_DATA (0xc01) /* finn: Evaluated from "(FINN_NV01_ROOT_VGPU_INTERFACE_ID << 8) | NV0000_CTRL_VGPU_GET_START_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_VGPU_GET_START_DATA_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_VGPU_GET_START_DATA_PARAMS {
    NvU8  mdevUuid[VM_UUID_SIZE];
    NvU8  configParams[1024];
    NvU32 qemuPid;
    NvU32 gpuPciId;
    NvU16 vgpuId;
    NvU32 gpuPciBdf;
} NV0000_CTRL_VGPU_GET_START_DATA_PARAMS;

/* _ctrl0000vgpu_h_ */
