/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Declarations for the VGPU event module.
//
//   Description:
//       This module declares the VGPU event interface functions/macros.
//
//******************************************************************************

#ifndef VGPU_EVENTS_H
#define VGPU_EVENTS_H

#include "rmconfig.h"

#include "ctrl/ctrlc637.h"
#include "ctrl/ctrl2080/ctrl2080bios.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080gr.h"
#include "ctrl/ctrl0080/ctrl0080nvjpg.h"

#include "vgpu/rpc_headers.h"

#include "gpu/device/device.h"

#include "vgpu/sdk-structures.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

typedef MC_ENGINE_BITVECTOR *PMC_ENGINE_BITVECTOR;
typedef struct HOST_VGPU_DEVICE HOST_VGPU_DEVICE, KERNEL_HOST_VGPU_DEVICE;
typedef struct _object_vgpu OBJVGPU, *POBJVGPU;

// Create and destroy OBJVGPU *object
NV_STATUS vgpuCreateObject(OBJGPU *pGpu);
void      vgpuDestructObject(OBJGPU *pGpu);

// Check if a VGPU event is pending
NvBool vgpuGetPendingEvent(OBJGPU *pGpu, THREAD_STATE_NODE *pThreadState);

// Service VGPU events
void vgpuService(OBJGPU *pGpu);

// Overwrite registry keys
void vgpuInitRegistryOverWrite(OBJGPU *pGpu);

// Get the device pointer from the calling context
Device *vgpuGetCallingContextDevice(OBJGPU *pGpu);

// Stubs for virtualization-disabled builds
static inline NV_STATUS vgpuGetCallingContextHostVgpuDevice(OBJGPU *pGpu, HOST_VGPU_DEVICE **ppHostVgpuDevice)
{
    *ppHostVgpuDevice = NULL;
    return NV_OK;
}

static inline NV_STATUS vgpuGetCallingContextKernelHostVgpuDevice(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice)
{
    *ppKernelHostVgpuDevice = NULL;
    return NV_OK;
}

static inline NV_STATUS vgpuGetCallingContextGfid(OBJGPU *pGpu, NvU32 *pGfid)
{
    *pGfid = GPU_GFID_PF;
    return NV_OK;
}

static inline NV_STATUS vgpuIsCallingContextPlugin(OBJGPU *pGpu, NvBool *pIsCallingContextPlugin)
{
    *pIsCallingContextPlugin = NV_FALSE;
    return NV_OK;
}

static inline NV_STATUS vgpuGetGfidFromDeviceInfo(OBJGPU *pGpu, Device *pDevice, NvU32 *pGfid)
{
    *pGfid = GPU_GFID_PF;
    return NV_OK;
}

// Update Interrupt using shared memory through vGPU
void vgpuUpdateShmIntr(OBJGPU *pGpu, NvU32 offset, NvU32 value, THREAD_STATE_NODE *pThreadState);

// Check if SW stalling interrupt is pending, using shared memory
NV_STATUS vgpuShmIsSwPending(OBJGPU *pGpu, NvU32 *isSwPending);

// Check if non-stalling interrupts are enabled, using shared memory
NV_STATUS vgpuShmIsNonStallEnabled(OBJGPU *pGpu, NvU32 *isNonStallEnabled);

// Check if non-stall interrupts are pening, using shared memory
NV_STATUS vgpuIsNonStallPending(OBJGPU *pGpu, PMC_ENGINE_BITVECTOR pEngines);

// Service non-stalling interrupts using shared memory
NV_STATUS vgpuServiceNonStall(OBJGPU *pGpu, PMC_ENGINE_BITVECTOR pEngines);

// Initialize and free event infrastructure
NV_STATUS _setupEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
NV_STATUS _teardownEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
NV_STATUS _setupGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
void _teardownGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);

#endif // VGPU_EVENTS_H
